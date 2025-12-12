// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Game_Info.h"
#include "AttributeSet.h"
#include "AttributeMacro.h"
#include "CharacterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCharacterAttributeSet();

public:
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Health);
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Mana);
	FOnManaChanged OnManaChanged;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxMana);

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Attack);

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Defense);

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MoveSpeed);

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData CriticalRate;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, CriticalRate);

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, CriticalChance);

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData SkillRate;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, SkillRate);

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData UltimateRate;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, UltimateRate);

public:
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data);
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue);

	

};
