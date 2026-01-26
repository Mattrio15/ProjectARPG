// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Game_Info.h"
#include "Engine/GameInstance.h"
#include "SaveGame/MySaveGame.h"
#include "MyGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveFinished, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadFinished, bool, bSuccess);


UCLASS()
class PROJECTARPG_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMyGameInstance();

protected:
	UPROPERTY(BlueprintReadOnly)
	bool mHasSave = false;
	bool mIsNewGame = true;

	FSaveGameData mSaveData;

public:
	virtual void Init();


public:
	UFUNCTION(BlueprintCallable)
	void ChangeLevel(TSoftObjectPtr<UWorld> Level, bool IsNewGame);

	UPROPERTY(BlueprintAssignable)
	FOnSaveFinished OnSaveFinished;
	UPROPERTY(BlueprintAssignable)
	FOnLoadFinished OnLoadFinished;

	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame();

	bool GetIsNewGame() { return mIsNewGame; }
	bool GetHasSave() { return mHasSave; }
	const FSaveGameData& GetSaveGameData() { return mSaveData; }

protected:
	void OnAsyncSaveFinished(const FString& SlotName, int32 UserIndex, bool bSuccess);
	void OnAsyncLoadFinished(const FString& SlotName, int32 UserIndex, USaveGame* LoadedGame);

};
