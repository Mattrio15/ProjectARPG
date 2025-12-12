// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageCalculation.h"

UDamageCalculation::UDamageCalculation()
{
	static ConstructorHelpers::FObjectFinder<UAttributeDataAsset>
		DataAsset(TEXT("/Script/ProjectARPG.AttributeDataAsset'/Game/GAS/DA_AttributeTag.DA_AttributeTag'"));
	if (DataAsset.Succeeded())
		mADA = DataAsset.Object;
}

void UDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();

	UAbilitySystemComponent* Source = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* Target = ExecutionParams.GetTargetAbilitySystemComponent();

	float Damage = 0;

	const UCharacterAttributeSet* SourceCAS = Source->GetSet<UCharacterAttributeSet>();
	const UMonsterAttributeSet* TargetMAS = Target->GetSet<UMonsterAttributeSet>();

	if (IsValid(SourceCAS) && IsValid(TargetMAS))
	{
		Damage = SourceCAS->GetAttack();
		Damage *= FMath::RandRange(0.8, 1.2);
		if (FMath::RandRange(0.0, 1.0) < SourceCAS->GetCriticalChance())
			Damage *= SourceCAS->GetCriticalRate();
		Damage -= TargetMAS->GetDefense();
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMonsterAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -Damage));
	}

	const UMonsterAttributeSet* SourceMAS = Source->GetSet<UMonsterAttributeSet>();
	const UCharacterAttributeSet* TargetCAS = Target->GetSet<UCharacterAttributeSet>();

	if (IsValid(SourceMAS) && IsValid(TargetCAS))
	{
		Damage = SourceMAS->GetAttack() - TargetCAS->GetDefense();
		Damage *= FMath::RandRange(0.8, 1.2);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -Damage));
	}

}
