// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributeSet.h"
#include "GameplayEffectExtension.h"

UCharacterAttributeSet::UCharacterAttributeSet()
{
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attribute = Data.EvaluatedData.Attribute;

	if (Attribute == GetHealthAttribute())
	{
		Health.SetCurrentValue(FMath::Clamp(Health.GetCurrentValue(), 0, MaxHealth.GetBaseValue()));
		OnHealthChanged.Broadcast(GetHealth() / GetMaxHealth());
	}

	else if (Attribute == GetMaxHealthAttribute())
		MaxHealth.SetCurrentValue(FMath::Clamp(MaxHealth.GetCurrentValue(), 0, MaxHealth.GetBaseValue()));

	else if (Attribute == GetManaAttribute())
	{
		Mana.SetCurrentValue(FMath::Clamp(Mana.GetCurrentValue(), 0, MaxMana.GetBaseValue()));
		OnManaChanged.Broadcast(GetMana() / GetMaxMana());
	}

	else if (Attribute == GetMaxManaAttribute())
		MaxMana.SetCurrentValue(FMath::Max(0.f, 100));

	else if (Attribute == GetAttackAttribute())
		Attack.SetCurrentValue(FMath::Max(0, Attack.GetCurrentValue()));

	else if (Attribute == GetDefenseAttribute())
		Defense.SetCurrentValue(FMath::Max(0, Defense.GetCurrentValue()));

	else if (Attribute == GetMoveSpeedAttribute())
		MoveSpeed.SetCurrentValue(FMath::Max(0, MoveSpeed.GetCurrentValue()));

	else if (Attribute == GetCriticalRateAttribute())
		CriticalRate.SetCurrentValue(FMath::Max(0, CriticalRate.GetCurrentValue()));

	else if (Attribute == GetCriticalChanceAttribute())
		CriticalChance.SetCurrentValue(FMath::Max(0, CriticalChance.GetCurrentValue()));

	else if (Attribute == GetSkillRateAttribute())
		SkillRate.SetCurrentValue(FMath::Max(0, SkillRate.GetCurrentValue()));

	else if (Attribute == GetUltimateRateAttribute())
		UltimateRate.SetCurrentValue(FMath::Max(0, UltimateRate.GetCurrentValue()));

}

void UCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetHealthAttribute())
		OnHealthChanged.Broadcast(GetHealth() / GetMaxHealth());
	else if (Attribute == GetManaAttribute())
		OnManaChanged.Broadcast(GetMana() / GetMaxMana());

}
