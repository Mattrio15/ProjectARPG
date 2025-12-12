// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "Engine/DataAsset.h"
#include "AttributeDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UAttributeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Attribute Tag")
	FGameplayTag mHealthTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Attribute Tag")
	FGameplayTag mMaxHealthTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Attribute Tag")
	FGameplayTag mManaTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Attribute Tag")
	FGameplayTag mMaxManaTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Attribute Tag")
	FGameplayTag mAttackTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Attribute Tag")
	FGameplayTag mDefenseTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Attribute Tag")
	FGameplayTag mMoveSpeedTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Attribute Tag")
	FGameplayTag mCriticalRateTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Attribute Tag")
	FGameplayTag mCriticalChanceTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Attribute Tag")
	FGameplayTag mSkillRateTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Attribute Tag")
	FGameplayTag mUltimateRateTag;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster Attribute Tag")
	FGameplayTag mMonsterHealthTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster Attribute Tag")
	FGameplayTag mMonsterAttackTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster Attribute Tag")
	FGameplayTag mMonsterDefenseTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster Attribute Tag")
	FGameplayTag mMonsterMoveSpeedTag;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage Tag")
	FGameplayTag mCharacterDamageTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage Tag")
	FGameplayTag mMonsterDamageTag;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster State Tag")
	FGameplayTag mElementalTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster State Tag")
	FGameplayTag mElemental_IceTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster State Tag")
	FGameplayTag mElemental_DarkTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster State Tag")
	FGameplayTag mElemental_ElectricTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster State Tag")
	FGameplayTag mElemental_BleedTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster State Tag")
	FGameplayTag mElemental_LightTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster State Tag")
	FGameplayTag mElemental_FireTag;


};
