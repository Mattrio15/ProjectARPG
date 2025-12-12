// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillDamageCalculation.h"

USkillDamageCalculation::USkillDamageCalculation()
{
}

void USkillDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* CASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* MASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const UCharacterAttributeSet* CAS = CASC->GetSet<UCharacterAttributeSet>();
	const UMonsterAttributeSet* MAS = MASC->GetSet<UMonsterAttributeSet>();

	if (!IsValid(CAS) || !IsValid(MAS))
		return;

	float Damage = CAS->GetAttack() * CAS->GetSkillRate();
	Damage *= FMath::RandRange(0.8, 1.2);
	if (FMath::RandRange(0.0, 1.0) < CAS->GetCriticalChance())
		Damage *= CAS->GetCriticalRate();

	Damage -= MAS->GetDefense();

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(MAS->GetHealthAttribute(), EGameplayModOp::Additive, -Damage));
}
