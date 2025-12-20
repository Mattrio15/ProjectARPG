// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "../GAS/AttributeSet/CharacterAttributeSet.h"
#include "CharacterInfo.h"
#include "../GAS/AttributeDataAsset.h"
#include "../GAS/GameplayAbility/AbilityDataAsset.h"
#include "DA_CharacterGE.h"
#include "../UI/Character/CharacterHPWidget.h"
#include "../UI/Character/CharacterInventory.h"
#include "ItemComponent.h"
#include "GameFramework/PlayerState.h"
#include "CharacterState.generated.h"

/**
 * 
 */
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

	TSubclassOf<UCharacterHPWidget> mHPWidgetClass;
	UCharacterHPWidget* mHPWidget;
	UCharacterInventory* mInventory;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UItemComponent> mItemComponent;
	
	TSubclassOf<UGameplayAbility> mGA_Dodge;

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

	void ShowUI(bool A);
	void ShowInventory(bool A);
	UCharacterHPWidget* GetMainWidget() { return mHPWidget; }

	void PlayButtonAnimation(int32 Index);
	void PlayQuickSlotAnimation(int32 Index);

public:
	FName GetCharacterName() { return mCharacterName; }
	void SetCharacterName(FName Name) { mCharacterName = Name; }

	UItemComponent* GetItemComponent() { return mItemComponent; }

	UFUNCTION(BlueprintCallable)
	void GetItem(UItemDataAsset* Item);

};
