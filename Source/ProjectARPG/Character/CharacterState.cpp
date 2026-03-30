// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterState.h"
#include "CharacterBase.h"
#include "ItemComponent.h"
#include "DA_CharacterGE.h"
#include "../GAS/AttributeSet/CharacterAttributeSet.h"
#include "../GAS/AttributeDataAsset.h"
#include "../GAS/GameplayAbility/AbilityDataAsset.h"
#include "../AI/Monster/MonsterBase.h"
#include "../SaveGame/MySaveGame.h"
#include "../UI/Character/CharacterHPWidget.h"
#include "../UI/Character/CharacterInventory.h"
#include "../UI/MiniGame/MiniGameWidget.h"
#include "../MyGameInstance.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"

ACharacterState::ACharacterState()
{
	mASC = MyObject(UAbilitySystemComponent, "ASC");
	mASC->SetIsReplicated(true); // 네트워크 동기화
	mASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // 동기화 방법

	mAS_Character = MyObject(UCharacterAttributeSet, "CAS");
		
	ObjectFinder(UDataTable, DT_CharacterInfo, "/Script/Engine.DataTable'/Game/GAS/GameplayEffect/Character/DT_CharacterInfo.DT_CharacterInfo'");
	if (DT_CharacterInfo.Succeeded())
		mDT_CharacterInfo = DT_CharacterInfo.Object;
		
	ObjectFinder(UAttributeDataAsset, DA_AttributeTag, "/Script/ProjectARPG.AttributeDataAsset'/Game/GAS/DA_AttributeTag.DA_AttributeTag'");
	if (DA_AttributeTag.Succeeded())
		mDA_AttributeTag = DA_AttributeTag.Object;

	ObjectFinder(UDA_CharacterGE, DA_CharacterGE, "/Script/ProjectARPG.DA_CharacterGE'/Game/GAS/GameplayEffect/Character/DA_CharacterGE.DA_CharacterGE'");
	if (DA_CharacterGE.Succeeded())
		mDA_CharacterGE = DA_CharacterGE.Object;

	ObjectFinder(UDataTable, DT_CharacterGE, "/Script/Engine.DataTable'/Game/GAS/GameplayEffect/Character/DT_CharacterGE.DT_CharacterGE'");
	if (DT_CharacterGE.Succeeded())
		mDT_CharacterGE = DT_CharacterGE.Object;

	ObjectFinder(UAbilityDataAsset, DA_CharacterGA, "/Script/ProjectARPG.AbilityDataAsset'/Game/GAS/GameplayAbility/DA_CharacterAbility.DA_CharacterAbility'");
	if (DA_CharacterGA.Succeeded())
		mDA_CharacterGA = DA_CharacterGA.Object;

	ClassFinder(UUserWidget, MainWidgetClass, "/Script/UMGEditor.WidgetBlueprint'/Game/UI/Character/WB_MainWidget.WB_MainWidget_C'");
	if (MainWidgetClass.Succeeded())
		mMainWidgetClass = MainWidgetClass.Class;

	mItemComponent = MyObject(UItemComponent, "ItemComponent");

	ClassFinder(UGameplayAbility, GA_Dodge, "/Script/Engine.Blueprint'/Game/GAS/GameplayAbility/Character/GA_Dodge.GA_Dodge_C'");
	if (GA_Dodge.Succeeded())
		mGA_DodgeClass = GA_Dodge.Class;

	ClassFinder(UGameplayEffect, GE_HPRegen, "/Script/Engine.Blueprint'/Game/GAS/GameplayEffect/Character/GE_Mana.GE_Mana_C'");
	if (GE_HPRegen.Succeeded())
		mGE_HPRegenClass = GE_HPRegen.Class;

}

void ACharacterState::BeginPlay()
{
	Super::BeginPlay();

	InitWidget();
	InitLevelSequence();
	InitCharacterGE();

}

void ACharacterState::InitWidget()
{
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

			mAS_Character->OnHealthChanged.AddUObject(mHPWidget, &UCharacterHPWidget::SetHPBar);
			mAS_Character->OnManaChanged.AddUObject(mHPWidget, &UCharacterHPWidget::SetMPBar);

			mInventoryWidget = Cast<UCharacterInventory>(mHPWidget->GetWidgetFromName(TEXT("WB_CharacterInventory")));
			if (IsValid(mInventoryWidget))
			{
				mInventoryWidget->SetVisibility(ESlateVisibility::Hidden);
				mInventoryWidget->SetItemComponent(mItemComponent);
				mItemComponent->SetAbilitySystemComponent(mASC);
				mItemComponent->SetInventory(mInventoryWidget);
				mItemComponent->SetMainWidget(mHPWidget);
			}

			SetCharacterFace();
		}
	}
}

void ACharacterState::InitLevelSequence()
{
	UMyGameInstance* GI = GetGameInstance<UMyGameInstance>();
	if (!IsValid(GI))
		return;

	if (!GI->GetIsNewGame())
	{
		ULevelSequence* LS = LoadObject<ULevelSequence>(GetWorld(), TEXT("/Script/LevelSequence.LevelSequence'/Game/LevelSequence/LS_Start.LS_Start'"));
		if (IsValid(LS))
		{
			FMovieSceneSequencePlaybackSettings Setting;
			Setting.bHideHud = true;
			ALevelSequenceActor* LSA = nullptr;
			ULevelSequencePlayer* LSP = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS, Setting, LSA);
			if (IsValid(LSP))
			{
				FInputModeUIOnly Mode;
				GetPlayerController()->SetInputMode(Mode);

				ShowMainWidget(false);
				LSP->Play();
				FQualifiedFrameTime Duration = LSP->GetDuration();
				FTimerHandle Timer;
				TWeakObjectPtr<ACharacterState> WeakThis(this);
				GetWorld()->GetTimerManager().SetTimer(Timer,
					[WeakThis]()
					{
						if (WeakThis.IsValid())
						{
							WeakThis->ShowMainWidget(true);
							FInputModeGameOnly Mode;
							APlayerController* PC = WeakThis->GetPlayerController();
							if (IsValid(PC))
							{
								PC->SetInputMode(Mode);
								PC->SetViewTarget(WeakThis->GetPawn());
							}
						}
					},
					Duration.AsSeconds(), false);
			}

		}
	}
}

void ACharacterState::InitCharacterGE()
{
	if (IsValid(mDT_CharacterGE))
	{
		TArray<FName> TA_Name = mDT_CharacterGE->GetRowNames();
		for (int32 i = 0; i < TA_Name.Num(); ++i)
		{
			FName Name = TA_Name[i];
			if (!mTM_CharacterGE.Contains(Name))
			{
				FCharacterGE* CharacterGE = mDT_CharacterGE->FindRow<FCharacterGE>(Name, TEXT(""));
				mTM_CharacterGE.Add(Name, *CharacterGE);
			}
		}
	}
}

void ACharacterState::InitAbilitySystemComponent(AActor* Avatar)
{
	if (!IsValid(mASC)) // ASC 확인
	{
		Log(TEXT("Ability System Component Is InValid"));
		return;
	}
	mASC->InitAbilityActorInfo(this, Avatar); // ASC의 아바타 액터 설정

	SetTMCharacterGE(); // 맵에 캐릭터 정보 저장

	SetCharacterMoveSpeed(); // 캐릭터 이동속도 설정

	ApplyGE_Init(); // GE_Init 실행

	ApplyCharacterGA(); // GA 부여

	SetCharacterFace(); // 캐릭터 초상화 설정
}

void ACharacterState::SetTMCharacterGE()
{
	FCharacterInfo* InfoPtr = mDT_CharacterInfo->FindRow<FCharacterInfo>(mCharacterName, TEXT("")); // 캐릭터 정보 가져오기
	if (!InfoPtr)
		return;

	if (!mTM_CharacterInfo.Contains(mCharacterName)) // 캐릭터 정보를 맵에 저장
		mTM_CharacterInfo.Add(mCharacterName, *InfoPtr);

	mTM_CharacterInfo[mCharacterName].CharacterFace = InfoPtr->CharacterFace; // 캐릭터 초상화 저장
}

void ACharacterState::SetCharacterMoveSpeed()
{
	FCharacterInfo Info = mTM_CharacterInfo[mCharacterName]; // 맵에 저장되어 있는 캐릭터 정보 사용

	ACharacterBase* CB = Cast<ACharacterBase>(mASC->GetAvatarActor()); // 캐릭터
	if (IsValid(CB))
	{
		if (Info.CharacterInfo.Contains(mDA_AttributeTag->mMoveSpeedTag)) // 캐릭터 이동속도 조절
			CB->GetCharacterMovement()->MaxWalkSpeed = Info.CharacterInfo[mDA_AttributeTag->mMoveSpeedTag];
	}
}

void ACharacterState::ApplyGE_Init()
{
	FCharacterInfo Info = mTM_CharacterInfo[mCharacterName]; // 맵에 저장되어 있는 캐릭터 정보 사용

	FGameplayEffectContextHandle Context = mASC->MakeEffectContext(); // GE 실행 용 Context
	if (IsValid(mDA_CharacterGE->mGE_Init))
	{
		FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(mDA_CharacterGE->mGE_Init, 1, Context); // 캐릭터 정보 초기화 용 GE Spec

		for (auto& Data : Info.CharacterInfo) // 캐릭터 정보와 맞게 GE에게 넘겨줌
			Spec.Data->SetSetByCallerMagnitude(Data.Key, Data.Value);

		mASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()); // 캐릭터 정보를 GE로 초기화
	}

	if (IsValid(mGE_HPRegenClass)) // 체력 자동 회복 GE
	{
		FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(mGE_HPRegenClass, 1, Context);
		mASC->ApplyGameplayEffectSpecToSelf(*(Spec.Data.Get()));
	}
}

void ACharacterState::ApplyCharacterGA()
{
	mASC->ClearAllAbilities(); // 가지고 있는 어빌리티 삭제
	if (mDA_CharacterGA->mCharacterAbilityData.Contains(mCharacterName)) // 캐릭터가 가지고 있는 어빌리티 확인 후 부여
	{
		for (auto& Ability : mDA_CharacterGA->mCharacterAbilityData[mCharacterName].CharacterAbility)
		{
			if (IsValid(Ability))
			{
				FGameplayAbilitySpec Spec = FGameplayAbilitySpec(Ability);
				mASC->GiveAbility(Spec);
			}
		}
	}

	FGameplayAbilitySpec DodgeSpec = FGameplayAbilitySpec(mGA_DodgeClass); // 회피 어빌리티 부여
	mASC->GiveAbility(mGA_DodgeClass);
}

void ACharacterState::SaveCharacterInfo()
{
	const UCharacterAttributeSet* CAS = mASC->GetSet<UCharacterAttributeSet>();

	FCharacterInfo SaveInfo;

	SaveInfo.CharacterInfo.Add(mDA_AttributeTag->mHealthTag);
	SaveInfo.CharacterInfo[mDA_AttributeTag->mHealthTag] = CAS->GetHealth();

	SaveInfo.CharacterInfo.Add(mDA_AttributeTag->mManaTag);
	SaveInfo.CharacterInfo[mDA_AttributeTag->mManaTag] = CAS->GetMana();

	SaveInfo.CharacterInfo.Add(mDA_AttributeTag->mAttackTag);
	SaveInfo.CharacterInfo[mDA_AttributeTag->mAttackTag] = CAS->GetAttack();

	SaveInfo.CharacterInfo.Add(mDA_AttributeTag->mDefenseTag);
	SaveInfo.CharacterInfo[mDA_AttributeTag->mDefenseTag] = CAS->GetDefense();

	SaveInfo.CharacterInfo.Add(mDA_AttributeTag->mMoveSpeedTag);
	SaveInfo.CharacterInfo[mDA_AttributeTag->mMoveSpeedTag] = CAS->GetMoveSpeed();

	SaveInfo.CharacterInfo.Add(mDA_AttributeTag->mCriticalRateTag);
	SaveInfo.CharacterInfo[mDA_AttributeTag->mCriticalRateTag] = CAS->GetCriticalRate();

	SaveInfo.CharacterInfo.Add(mDA_AttributeTag->mCriticalChanceTag);
	SaveInfo.CharacterInfo[mDA_AttributeTag->mCriticalChanceTag] = CAS->GetCriticalChance();

	SaveInfo.CharacterInfo.Add(mDA_AttributeTag->mSkillRateTag);
	SaveInfo.CharacterInfo[mDA_AttributeTag->mSkillRateTag] = CAS->GetSkillRate();

	SaveInfo.CharacterInfo.Add(mDA_AttributeTag->mUltimateRateTag);
	SaveInfo.CharacterInfo[mDA_AttributeTag->mUltimateRateTag] = CAS->GetUltimateRate();

	SaveInfo.CharacterFace = mTM_CharacterInfo[mCharacterName].CharacterFace;

	mTM_CharacterInfo[mCharacterName] = SaveInfo;

}

void ACharacterState::PlayGE_Attack(FName Name, UAbilitySystemComponent* ASC)
{
	FCharacterGE* CharacterGE = mTM_CharacterGE.Find(Name);
	if (CharacterGE)
	{
		TSubclassOf<UGameplayEffect> GE_Attack = CharacterGE->GE_Attack;
		if (IsValid(GE_Attack))
		{
			FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(GE_Attack, 1, Context);
			mASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), ASC);
		}
		FGameplayTag GT = CharacterGE->GT_Elemental;
		if (!ASC->HasMatchingGameplayTag(GT))
		{
			AMonsterBase* Monster = Cast<AMonsterBase>(ASC->GetAvatarActor());
			if (IsValid(Monster))
				Monster->SetElemental(GT);
		}
	}

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
	if (mDA_CharacterGE->mGE_CounterAttack.Contains(Name))
	{
		if (IsValid(mDA_CharacterGE->mGE_CounterAttack[Name]))
		{
			FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(mDA_CharacterGE->mGE_CounterAttack[Name], 1, Context);
			mASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), ASC);
		}
	}

	if (mDA_CharacterGE->mGE_Elemental_Tag.Contains(Name))
		if (!ASC->HasMatchingGameplayTag(mDA_CharacterGE->mGE_Elemental_Tag[Name]))
		{
			AMonsterBase* Monster = Cast<AMonsterBase>(ASC->GetAvatarActor());
			if (IsValid(Monster))
				Monster->SetElemental(mDA_CharacterGE->mGE_Elemental_Tag[Name]);
		}

}

void ACharacterState::PlayGE_Skill(FName Name, UAbilitySystemComponent* ASC)
{
	if (mDA_CharacterGE->mGE_Skill.Contains(Name))
	{
		if (IsValid(mDA_CharacterGE->mGE_Skill[Name]))
		{
			FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(mDA_CharacterGE->mGE_Skill[Name], 1, Context);
			mASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), ASC);
		}
	}
	if (mDA_CharacterGE->mGE_Elemental_Tag.Contains(Name))
	{
		if (!ASC->HasMatchingGameplayTag(mDA_CharacterGE->mGE_Elemental_Tag[Name]))
		{
			AMonsterBase* Monster = Cast<AMonsterBase>(ASC->GetAvatarActor());
			if (IsValid(Monster))
				Monster->SetElemental(mDA_CharacterGE->mGE_Elemental_Tag[Name]);
		}
	}

}

bool ACharacterState::PlayGA_Dodge()
{
	return mASC->TryActivateAbilityByClass(mGA_DodgeClass);
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
	if (IsValid(mInventoryWidget))
	{
		if (A)
			mInventoryWidget->SetVisibility(ESlateVisibility::Visible);
		else
			mInventoryWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
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
		mHPWidget->SetCharacterFace(mTM_CharacterInfo[mCharacterName].CharacterFace);

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

		for (auto& Map : mTM_CharacterInfo)
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
		if(mTM_CharacterInfo.Contains(Map.Key))
			mTM_CharacterInfo[Map.Key].CharacterInfo = Map.Value.CharacterState;
		else
		{
			FCharacterInfo Info;
			Info.CharacterInfo = Map.Value.CharacterState;
			mTM_CharacterInfo.Add(Map.Key, Info);
		}
	}
}
