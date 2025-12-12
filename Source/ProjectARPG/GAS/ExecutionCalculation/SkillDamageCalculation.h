// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Calculation_Info.h"
#include "GameplayEffectExecutionCalculation.h"
#include "SkillDamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API USkillDamageCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	USkillDamageCalculation();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const;
	
};
