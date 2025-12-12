// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Game_Info.h"
#include "AttributeSet.h"
#include "AttributeMacro.h"
#include "../AttributeDataAsset.h"
#include "ElementalDataAsset.h"
#include "MonsterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UMonsterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMonsterAttributeSet();

public:
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, Health);
	FOnHealthChanged OnHealthChanged;
	FOnHitDamage OnHitDamage;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, Attack);

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, Defense);

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, MoveSpeed);
	FOnMoveSpeedChanged OnMoveSpeedChanged;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData ElementalRating;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, ElementalRating);
	FOnElementalChanged OnElementalChanged;
	FOnElementalOver OnElementalOver;


public:
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data);
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue);


	
};
