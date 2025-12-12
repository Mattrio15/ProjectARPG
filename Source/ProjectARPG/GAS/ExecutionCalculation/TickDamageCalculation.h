// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Calculation_Info.h"
#include "GameplayEffectExecutionCalculation.h"
#include "TickDamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UTickDamageCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UTickDamageCalculation();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const;
	
};
