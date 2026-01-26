// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "CharacterInfo.h"
#include "GameFramework/PlayerState.h"
#include "CharacterState.generated.h"

struct FSaveGameData;
class UItemComponent;

class UCharacterAttributeSet;
class UAttributeDataAsset;
class UAbilityDataAsset;
class UDA_CharacterGE;

class UCharacterHPWidget;
class UCharacterInventory;
class UMiniGameWidget;

UCLASS()
class PROJECTARPG_API ACharacterState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ACharacterState();

protected:
	FName mCharacterName;
	TMap<FName, FCharacterInfo> mCharacterInfo;
	UDataTable* mCharacterInfoTable;

	UAbilitySystemComponent* mASC;
	UCharacterAttributeSet* mCAS;

	UAttributeDataAsset* mADA;

	UDA_CharacterGE* mCharacterGE;
	UAbilityDataAsset* mAbilityDataAsset;

	TSubclassOf<UUserWidget> mMainWidgetClass;
	UCharacterHPWidget* mHPWidget;
	UCharacterInventory* mInventory;
	UUserWidget* mMainWidget;
	UUserWidget* mPauseWidget;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UItemComponent> mItemComponent;
	
	TSubclassOf<UGameplayAbility> mGA_Dodge;

	UMiniGameWidget* mMiniGameWidget;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:
	virtual void BeginPlay();

protected:
	void SetElemental(UAbilitySystemComponent* ASC, FGameplayTag Tag);

public:
	void InitAbilitySystemComponent(AActor* Avatar);
	void SaveCharacterInfo();

	void PlayGE_Attack(FName Name, UAbilitySystemComponent* ASC);
	void PlayGE_CounterAttack(FName Name, UAbilitySystemComponent* ASC);
	void PlayGE_Skill(FName Name, UAbilitySystemComponent* ASC);

	bool PlayGA_Dodge();

	UFUNCTION(BlueprintCallable)
	void ShowMainWidget(bool A);
	void ShowUI(bool A);
	void ShowInventory(bool A);
	void ShowPause();
	void ShowFKey(bool A);

	void PlayButtonAnimation(int32 Index);
	void PlayQuickSlotAnimation(int32 Index);

	void PlayMiniGame();

public:
	UCharacterHPWidget* GetHPWidget() { return mHPWidget; }
	FName GetCharacterName() { return mCharacterName; }
	void SetCharacterName(FName Name) { mCharacterName = Name; }

	UItemComponent* GetItemComponent() { return mItemComponent; }

	UFUNCTION(BlueprintCallable)
	void GetItem(UItemDataAsset* Item);

	void SetCharacterFace();

public:
	FSaveGameData GetSaveGameData();

	void SetSaveGameData(FSaveGameData Data);


};
