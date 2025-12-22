// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterState.h"
#include "CharacterBase.h"
#include "../AI/Monster/MonsterBase.h"

ACharacterState::ACharacterState()
{
	mASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	mASC->SetIsReplicated(true); // 네트워크 동기화
	mASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // 동기화 방법

	mCAS = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("CAS"));

	static ConstructorHelpers::FObjectFinder<UDataTable>
		TableAsset(TEXT("/Script/Engine.DataTable'/Game/GAS/GameplayEffect/Character/DT_CharacterInfo.DT_CharacterInfo'"));
	if (TableAsset.Succeeded())
		mCharacterInfoTable = TableAsset.Object;

	static ConstructorHelpers::FObjectFinder<UAttributeDataAsset>
		DataAsset(TEXT("/Script/ProjectARPG.AttributeDataAsset'/Game/GAS/DA_AttributeTag.DA_AttributeTag'"));
	if (DataAsset.Succeeded())
		mADA = DataAsset.Object;

	static ConstructorHelpers::FObjectFinder<UDA_CharacterGE>
		GEAsset(TEXT("/Script/ProjectARPG.DA_CharacterGE'/Game/GAS/GameplayEffect/Character/DA_CharacterGE.DA_CharacterGE'"));
	if (GEAsset.Succeeded())
		mCharacterGE = GEAsset.Object;

	static ConstructorHelpers::FObjectFinder<UAbilityDataAsset>
		AbilityDataAsset(TEXT("/Script/ProjectARPG.AbilityDataAsset'/Game/GAS/GameplayAbility/DA_CharacterAbility.DA_CharacterAbility'"));
	if (AbilityDataAsset.Succeeded())
		mAbilityDataAsset = AbilityDataAsset.Object;

	static ConstructorHelpers::FClassFinder<UCharacterHPWidget>
		HPWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Character/WB_CharacterHP.WB_CharacterHP_C'"));
	if (HPWidget.Succeeded())
		mHPWidgetClass = HPWidget.Class;

	mItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));

	static ConstructorHelpers::FClassFinder<UGameplayAbility>
		GA_Dodge(TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayAbility/Character/GA_Dodge.GA_Dodge_C'"));
	if (GA_Dodge.Succeeded())
		mGA_Dodge = GA_Dodge.Class;

}

UAbilitySystemComponent* ACharacterState::GetAbilitySystemComponent() const
{
	return mASC;
}

void ACharacterState::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(mHPWidgetClass))
	{
		mHPWidget = CreateWidget<UCharacterHPWidget>(GetWorld(), mHPWidgetClass);
		if (IsValid(mHPWidget))
		{
			mHPWidget->AddToViewport();

			mCAS->OnHealthChanged.AddUObject(mHPWidget, &UCharacterHPWidget::SetHPBar);
			mCAS->OnManaChanged.AddUObject(mHPWidget, &UCharacterHPWidget::SetMPBar);

			mInventory = Cast<UCharacterInventory>(mHPWidget->GetWidgetFromName(TEXT("WB_CharacterInventory")));
			if (IsValid(mInventory))
			{
				mInventory->SetVisibility(ESlateVisibility::Hidden);
				mInventory->SetItemComponent(mItemComponent);
				mItemComponent->SetAbilitySystemComponent(mASC);
				mItemComponent->SetInventory(mInventory);
				mItemComponent->SetMainWidget(mHPWidget);
			}
		}
	}

}

void ACharacterState::SetElemental(UAbilitySystemComponent* ASC, FGameplayTag Tag)
{
	ASC->RemoveLooseGameplayTag(mADA->mElemental_IceTag);
	ASC->RemoveLooseGameplayTag(mADA->mElemental_DarkTag);
	ASC->RemoveLooseGameplayTag(mADA->mElemental_ElectricTag);
	ASC->RemoveLooseGameplayTag(mADA->mElemental_BleedTag);
	ASC->RemoveLooseGameplayTag(mADA->mElemental_LightTag);
	ASC->RemoveLooseGameplayTag(mADA->mElemental_FireTag);

	ASC->AddLooseGameplayTag(Tag);

	AMonsterBase* Monster = Cast<AMonsterBase>(ASC->GetAvatarActor());
	if (IsValid(Monster))
		Monster->SetElementalTexture();
}

void ACharacterState::InitAbilitySystemComponent(AActor* Avatar)
{
	if (!IsValid(mASC))
	{
		Log(TEXT("Ability System Component Is InValid"));
		return;
	}
	mASC->InitAbilityActorInfo(this, Avatar);

	if (!mCharacterInfo.Contains(mCharacterName))
	{
		if (!IsValid(mCharacterInfoTable))
		{
			Log(TEXT("CharacterInfoTable Is Invalid"));
			return;
		}
		FCharacterInfo* Info = mCharacterInfoTable->FindRow<FCharacterInfo>(mCharacterName, TEXT(""));
		mCharacterInfo.Add(mCharacterName, *Info);
	}

	FCharacterInfo Info = mCharacterInfo[mCharacterName];

	GetPawn<ACharacterBase>()->GetCharacterMovement()->MaxWalkSpeed = Info.CharacterInfo[mADA->mMoveSpeedTag];

	FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
	if (IsValid(mCharacterGE->mGE_Init))
	{
		FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(mCharacterGE->mGE_Init, 1, Context);

		for (auto& Data : Info.CharacterInfo)
			Spec.Data->SetSetByCallerMagnitude(Data.Key, Data.Value);

		mASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
	else
		Log(TEXT("GE_Init Is Invalid!"));

	mASC->ClearAllAbilities();
	if (mAbilityDataAsset->mCharacterAbilityData.Contains(mCharacterName))
	{
		for (auto& Ability : mAbilityDataAsset->mCharacterAbilityData[mCharacterName].CharacterAbility)
		{
			if (IsValid(Ability))
			{
				FGameplayAbilitySpec Spec = FGameplayAbilitySpec(Ability);
				mASC->GiveAbility(Spec);
			}
		}
	}

	FGameplayAbilitySpec DodgeSpec = FGameplayAbilitySpec(mGA_Dodge);
	mASC->GiveAbility(mGA_Dodge);

	TSubclassOf<UGameplayEffect> HPRegen = LoadClass<UGameplayEffect>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayEffect/Character/GE_HealthRegen.GE_HealthRegen_C'"));
	if (IsValid(HPRegen))
	{
		FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(HPRegen, 1, Context);
		mASC->ApplyGameplayEffectSpecToSelf(*(Spec.Data.Get()));
	}

	SetCharacterFace();
}

void ACharacterState::SaveCharacterInfo()
{
	const UCharacterAttributeSet* CAS = mASC->GetSet<UCharacterAttributeSet>();

	FCharacterInfo SaveInfo;

	SaveInfo.CharacterInfo.Add(mADA->mHealthTag);
	SaveInfo.CharacterInfo[mADA->mHealthTag] = CAS->GetHealth();

	SaveInfo.CharacterInfo.Add(mADA->mManaTag);
	SaveInfo.CharacterInfo[mADA->mManaTag] = CAS->GetMana();

	SaveInfo.CharacterInfo.Add(mADA->mAttackTag);
	SaveInfo.CharacterInfo[mADA->mAttackTag] = CAS->GetAttack();

	SaveInfo.CharacterInfo.Add(mADA->mDefenseTag);
	SaveInfo.CharacterInfo[mADA->mDefenseTag] = CAS->GetDefense();

	SaveInfo.CharacterInfo.Add(mADA->mMoveSpeedTag);
	SaveInfo.CharacterInfo[mADA->mMoveSpeedTag] = CAS->GetMoveSpeed();

	SaveInfo.CharacterInfo.Add(mADA->mCriticalRateTag);
	SaveInfo.CharacterInfo[mADA->mCriticalRateTag] = CAS->GetCriticalRate();

	SaveInfo.CharacterInfo.Add(mADA->mCriticalChanceTag);
	SaveInfo.CharacterInfo[mADA->mCriticalChanceTag] = CAS->GetCriticalChance();

	SaveInfo.CharacterInfo.Add(mADA->mSkillRateTag);
	SaveInfo.CharacterInfo[mADA->mSkillRateTag] = CAS->GetSkillRate();

	SaveInfo.CharacterInfo.Add(mADA->mUltimateRateTag);
	SaveInfo.CharacterInfo[mADA->mUltimateRateTag] = CAS->GetUltimateRate();

	SaveInfo.CharacterFace = mCharacterInfo[mCharacterName].CharacterFace;

	mCharacterInfo[mCharacterName] = SaveInfo;

}

void ACharacterState::PlayGE_Attack(FName Name, UAbilitySystemComponent* ASC)
{
	if (mCharacterGE->mGE_Attack.Contains(Name))
	{
		if (IsValid(mCharacterGE->mGE_Attack[Name]))
		{
			FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(mCharacterGE->mGE_Attack[Name], 1, Context);
			mASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), ASC);
		}
	}
	if (mCharacterGE->mGE_Elemental_Tag.Contains(Name))
		if (!ASC->HasMatchingGameplayTag(mCharacterGE->mGE_Elemental_Tag[Name]))
			SetElemental(ASC, mCharacterGE->mGE_Elemental_Tag[Name]);

	TSubclassOf<UGameplayEffect> Mana = LoadClass<UGameplayEffect>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayEffect/Character/GE_Mana.GE_Mana_C'"));
	if (IsValid(Mana))
	{
		FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
		FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(Mana, 1, Context);
		mASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}

void ACharacterState::PlayGE_CounterAttack(FName Name, UAbilitySystemComponent* ASC)
{
	if (mCharacterGE->mGE_CounterAttack.Contains(Name))
	{
		if (IsValid(mCharacterGE->mGE_CounterAttack[Name]))
		{
			FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(mCharacterGE->mGE_CounterAttack[Name], 1, Context);
			mASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), ASC);
		}
	}

	if (mCharacterGE->mGE_Elemental_Tag.Contains(Name))
		if (!ASC->HasMatchingGameplayTag(mCharacterGE->mGE_Elemental_Tag[Name]))
			SetElemental(ASC, mCharacterGE->mGE_Elemental_Tag[Name]);

}

void ACharacterState::PlayGE_Skill(FName Name, UAbilitySystemComponent* ASC)
{
	if (mCharacterGE->mGE_Skill.Contains(Name))
	{
		if (IsValid(mCharacterGE->mGE_Skill[Name]))
		{
			FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(mCharacterGE->mGE_Skill[Name], 1, Context);
			mASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), ASC);
		}
	}
	if (mCharacterGE->mGE_Elemental_Tag.Contains(Name))
		if (!ASC->HasMatchingGameplayTag(mCharacterGE->mGE_Elemental_Tag[Name]))
			SetElemental(ASC, mCharacterGE->mGE_Elemental_Tag[Name]);

}

bool ACharacterState::PlayGA_Dodge()
{
	return mASC->TryActivateAbilityByClass(mGA_Dodge);
}

void ACharacterState::ShowUI(bool A)
{
	ShowInventory(A);
}

void ACharacterState::ShowInventory(bool A)
{
	if (IsValid(mInventory))
	{
		if (A)
			mInventory->SetVisibility(ESlateVisibility::Visible);
		else
			mInventory->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void ACharacterState::PlayButtonAnimation(int32 Index)
{
	if (IsValid(mHPWidget))
		mHPWidget->PlayButtonAnimation(Index);
}

void ACharacterState::PlayQuickSlotAnimation(int32 Index)
{
	if (IsValid(mHPWidget))
		mHPWidget->PlayQuickSlotAnimation(Index);
}

void ACharacterState::GetItem(UItemDataAsset* Item)
{
	mItemComponent->AddItem(Item);
}

void ACharacterState::SetCharacterFace()
{
	if (IsValid(mHPWidget))
		mHPWidget->SetCharacterFace(mCharacterInfo[mCharacterName].CharacterFace);

}
