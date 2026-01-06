// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Game_Info.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMyGameInstance();

protected:
	UPROPERTY(BlueprintReadOnly)
	bool mHasSave = false;

public:
	virtual void Init();


public:
	UFUNCTION(BlueprintCallable)
	void SetHasSave(bool A) { mHasSave = A; }

	UFUNCTION(BlueprintCallable)
	void ChangeLevel(TSoftObjectPtr<UWorld> Level);

	
};
