// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Calculation_Info.h"
#include "GameplayEffectExecutionCalculation.h"
#include "../AttributeDataAsset.h"
#include "InitialAttributeCalculation.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UInitialAttributeCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UInitialAttributeCalculation();

protected:
	UAttributeDataAsset* mADA;

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const;
	
};
