// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Game_Info.h"
#include "Engine/DataAsset.h"
#include "ElementalDataAsset.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FElementalEffectData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ElementalTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ElementalEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture* ElementalTexture;

};

UCLASS()
class PROJECTARPG_API UElementalDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FElementalEffectData> mElementalEffectDataArray;
	
};
