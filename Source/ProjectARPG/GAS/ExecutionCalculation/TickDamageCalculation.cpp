// Fill out your copyright notice in the Description page of Project Settings.


#include "TickDamageCalculation.h"

UTickDamageCalculation::UTickDamageCalculation()
{
}

void UTickDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* CharacterASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* MonsterASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const UCharacterAttributeSet* CharacterAS = CharacterASC->GetSet<UCharacterAttributeSet>();
	const UMonsterAttributeSet* MonsterAS = MonsterASC->GetSet<UMonsterAttributeSet>();

	float Damage = CharacterAS->GetAttack() * CharacterAS->GetSkillRate();
	Damage *= FMath::RandRange(0.8, 1.2);
	if (FMath::RandRange(0.0, 1.0) < CharacterAS->GetCriticalChance())
		Damage *= CharacterAS->GetCriticalRate();
	Damage /= 2;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(MonsterAS->GetHealthAttribute(), EGameplayModOp::Additive, -Damage));
}
