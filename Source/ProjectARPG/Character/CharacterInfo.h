// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "Engine/DataTable.h"
#include "CharacterInfo.generated.h"

USTRUCT(BlueprintType)
struct FCharacterInfo :public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, float> CharacterInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CharacterFace;

};