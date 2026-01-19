// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterState.h"
#include "CharacterBase.h"
#include "../AI/Monster/MonsterBase.h"
#include "../SaveGame/MySaveGame.h"
#include "../UI/MiniGame/MiniGameWidget.h"

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

	static ConstructorHelpers::FClassFinder<UUserWidget>
		MainWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Character/WB_MainWidget.WB_MainWidget_C'"));
	if (MainWidget.Succeeded())
		mMainWidgetClass = MainWidget.Class;

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

	if (IsValid(mMainWidgetClass))
	{
		mMainWidget = CreateWidget<UUserWidget>(GetWorld(), mMainWidgetClass);
		if (IsValid(mMainWidget))
		{
			mHPWidget = Cast<UCharacterHPWidget>(mMainWidget->GetWidgetFromName(TEXT("WB_CharacterHP")));
			mPauseWidget = Cast<UUserWidget>(mMainWidget->GetWidgetFromName(TEXT("WB_CharacterPause")));
		}
		if (IsValid(mHPWidget))
		{
			mMainWidget->AddToViewport();

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

			SetCharacterFace();
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

	FCharacterInfo* InfoPtr = mCharacterInfoTable->FindRow<FCharacterInfo>(mCharacterName, TEXT(""));
	if (!InfoPtr)
		return;

	if (!mCharacterInfo.Contains(mCharacterName))
		mCharacterInfo.Add(mCharacterName, *InfoPtr);
	
	mCharacterInfo[mCharacterName].CharacterFace = InfoPtr->CharacterFace;
	FCharacterInfo Info = mCharacterInfo[mCharacterName];

	ACharacterBase* CB = Cast<ACharacterBase>(Avatar);
	if (Info.CharacterInfo.Contains(mADA->mMoveSpeedTag))
		CB->GetCharacterMovement()->MaxWalkSpeed = Info.CharacterInfo[mADA->mMoveSpeedTag];
	else
		Test(TEXT("Empty!"));

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

void ACharacterState::ShowMainWidget(bool A)
{
	if (A)
		mMainWidget->SetVisibility(ESlateVisibility::Visible);
	else
		mMainWidget->SetVisibility(ESlateVisibility::Hidden);
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

void ACharacterState::ShowPause()
{
	if (!IsValid(mPauseWidget))
	{
		Log(TEXT("PauseWidget Is Invalid!"));
		return;
	}

	mPauseWidget->SetVisibility(ESlateVisibility::Visible);

	UGameplayStatics::SetGamePaused(GetWorld(), true);

	FInputModeUIOnly Mode;
	GetPlayerController()->SetInputMode(Mode);
	GetPlayerController()->bShowMouseCursor = true;

}

void ACharacterState::ShowFKey(bool A)
{
	if (!IsValid(mHPWidget))
		return;

	mHPWidget->ShowFKey(A);
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

void ACharacterState::PlayMiniGame()
{
	ShowMainWidget(false);

	APlayerController* PC = GetPlayerController();
	if (IsValid(PC))
	{
		FInputModeUIOnly Mode;
		PC->SetInputMode(Mode);
		PC->bShowMouseCursor = true;
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		if (!IsValid(mMiniGameWidget))
		{
			TSubclassOf<UMiniGameWidget> MGWClass = LoadClass<UMiniGameWidget>(GetWorld(), TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/MiniGame/WB_MiniGame.WB_MiniGame_C'"));
			if (IsValid(MGWClass))
			{
				mMiniGameWidget = CreateWidget<UMiniGameWidget>(GetWorld(), MGWClass);
			}
		}
		mMiniGameWidget->AddToViewport();
		mMiniGameWidget->SetVisibility(ESlateVisibility::Visible);
	}

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

FSaveGameData ACharacterState::GetSaveGameData()
{
	FSaveGameData Data;

	ACharacterBase* Character = GetPawn<ACharacterBase>();
	
	if (IsValid(Character))
	{
		Data.LevelName = TEXT("TestLevel");
		Data.CharacterName = mCharacterName;
		Data.CharacterLocation = Character->GetActorLocation();
		Data.CharacterRotation = Character->GetActorRotation();
		Data.CameraRotation = Character->GetCameraRotation();
		Data.DirYaw = Character->GetDirYaw();

		for (auto& Map : mCharacterInfo)
		{
			FCharacterStateInfo CSI;
			CSI.CharacterState = Map.Value.CharacterInfo;
			Data.CharacterInfo.Add(Map.Key, CSI);
		}
	}

	return Data;
}

void ACharacterState::SetSaveGameData(FSaveGameData Data)
{
	for (auto& Map : Data.CharacterInfo)
	{
		if(mCharacterInfo.Contains(Map.Key))
			mCharacterInfo[Map.Key].CharacterInfo = Map.Value.CharacterState;
		else
		{
			FCharacterInfo Info;
			Info.CharacterInfo = Map.Value.CharacterState;
			mCharacterInfo.Add(Map.Key, Info);
		}
	}
}
