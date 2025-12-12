// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Game_Info.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UItemDataAsset : public UDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FName mName;
	UPROPERTY(EditAnywhere)
	UTexture2D* mImage;
	UPROPERTY(EditAnywhere)
	FText mDescribe;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> mEffect;

public:
	FName GetItemName() { return mName; }
	UTexture2D* GetImage() { return mImage; }
	FText GetDescribe() { return mDescribe; }
	TSubclassOf<UGameplayEffect> GetGE() { return mEffect; }

};
