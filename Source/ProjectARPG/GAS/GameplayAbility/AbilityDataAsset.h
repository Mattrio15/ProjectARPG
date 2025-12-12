// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GA_Info.h"
#include "Engine/DataAsset.h"
#include "AbilityDataAsset.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCharacterAbilityData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> CharacterAbility;

};

UCLASS()
class PROJECTARPG_API UAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FCharacterAbilityData> mCharacterAbilityData;

};
