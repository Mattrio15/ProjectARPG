// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterInitCalculation.h"

UMonsterInitCalculation::UMonsterInitCalculation()
{
	static ConstructorHelpers::FObjectFinder<UAttributeDataAsset>
		DataAsset(TEXT("/Script/ProjectARPG.AttributeDataAsset'/Game/GAS/DA_AttributeTag.DA_AttributeTag'"));
	if (DataAsset.Succeeded())
		mADA = DataAsset.Object;
}

void UMonsterInitCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mMonsterHealthTag))
	{
		float Health = Spec.GetSetByCallerMagnitude(mADA->mMonsterHealthTag, false);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMonsterAttributeSet::GetHealthAttribute(), EGameplayModOp::Override, Health));
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMonsterAttributeSet::GetMaxHealthAttribute(), EGameplayModOp::Override, Health));
	}
	
	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mMonsterAttackTag))
	{
		float Attack = Spec.GetSetByCallerMagnitude(mADA->mMonsterAttackTag, false);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMonsterAttributeSet::GetAttackAttribute(), EGameplayModOp::Override, Attack));
	}

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mMonsterDefenseTag))
	{
		float Defense = Spec.GetSetByCallerMagnitude(mADA->mMonsterDefenseTag, false);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMonsterAttributeSet::GetDefenseAttribute(), EGameplayModOp::Override, Defense));
	}

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mMonsterMoveSpeedTag))
	{
		float MoveSpeed = Spec.GetSetByCallerMagnitude(mADA->mMonsterMoveSpeedTag, false);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMonsterAttributeSet::GetMoveSpeedAttribute(), EGameplayModOp::Override, MoveSpeed));
	}

}
