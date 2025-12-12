// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Calculation_Info.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UDamageCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UDamageCalculation();

protected:
	UAttributeDataAsset* mADA;

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const;
	
};
