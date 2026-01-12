// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStateInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, float> CharacterState;

};

USTRUCT(BlueprintType)
struct FSaveGameData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName LevelName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName CharacterName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector CharacterLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator CharacterRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator CameraRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float DirYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, FCharacterStateInfo> CharacterInfo;
};

UCLASS()
class PROJECTARPG_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UMySaveGame();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FSaveGameData mSaveGameData;

public:
	UFUNCTION(BlueprintCallable)
	void SetSaveGameData(const FSaveGameData& SaveGameData) { mSaveGameData = SaveGameData; }

public:
	UFUNCTION(BlueprintCallable)
	FSaveGameData GetSaveGameData() const { return mSaveGameData; }
	
};
