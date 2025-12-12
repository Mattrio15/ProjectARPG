// Fill out your copyright notice in the Description page of Project Settings.


#include "InitialAttributeCalculation.h"
#include "../../Character/CharacterInfo.h"
#include "../../Character/CharacterState.h"

UInitialAttributeCalculation::UInitialAttributeCalculation()
{
	static ConstructorHelpers::FObjectFinder<UAttributeDataAsset>
		DataAsset(TEXT("/Script/ProjectARPG.AttributeDataAsset'/Game/GAS/DA_AttributeTag.DA_AttributeTag'"));
	if (DataAsset.Succeeded())
		mADA = DataAsset.Object;
}

void UInitialAttributeCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mHealthTag))
	{
		float Health = Spec.GetSetByCallerMagnitude(mADA->mHealthTag, false, -1);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetMaxHealthAttribute(), EGameplayModOp::Override, Health));
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetHealthAttribute(), EGameplayModOp::Override, Health));
	}

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mMaxManaTag))
	{
		float MaxMana = Spec.GetSetByCallerMagnitude(mADA->mMaxManaTag, false, -1);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetMaxManaAttribute(), EGameplayModOp::Override, MaxMana));
	}

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mManaTag))
	{
		float Mana = Spec.GetSetByCallerMagnitude(mADA->mManaTag, false, -1);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetManaAttribute(), EGameplayModOp::Override, Mana));
	}

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mAttackTag))
	{
		float Attack = Spec.GetSetByCallerMagnitude(mADA->mAttackTag, false, -1);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetAttackAttribute(), EGameplayModOp::Override, Attack));
	}

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mDefenseTag))
	{
		float Defense = Spec.GetSetByCallerMagnitude(mADA->mDefenseTag, false, -1);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetDefenseAttribute(), EGameplayModOp::Override, Defense));
	}

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mMoveSpeedTag))
	{
		float MoveSpeed = Spec.GetSetByCallerMagnitude(mADA->mMoveSpeedTag, false, -1);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetMoveSpeedAttribute(), EGameplayModOp::Override, MoveSpeed));
	}

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mCriticalRateTag))
	{
		float CriticalRate = Spec.GetSetByCallerMagnitude(mADA->mCriticalRateTag, false, -1);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetCriticalRateAttribute(), EGameplayModOp::Override, CriticalRate));
	}

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mCriticalChanceTag))
	{
		float CriticalChance = Spec.GetSetByCallerMagnitude(mADA->mCriticalChanceTag, false, -1);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetCriticalChanceAttribute(), EGameplayModOp::Override, CriticalChance));
	}

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mSkillRateTag))
	{
		float SkillRate = Spec.GetSetByCallerMagnitude(mADA->mSkillRateTag, false, -1);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetSkillRateAttribute(), EGameplayModOp::Override, SkillRate));
	}

	if (Spec.SetByCallerTagMagnitudes.Contains(mADA->mUltimateRateTag))
	{
		float UltimateRate = Spec.GetSetByCallerMagnitude(mADA->mUltimateRateTag, false, -1);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetUltimateRateAttribute(), EGameplayModOp::Override, UltimateRate));
	}

}
