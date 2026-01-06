// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

UMyGameInstance::UMyGameInstance()
{
}

void UMyGameInstance::Init()
{
	Super::Init();

	mHasSave = UGameplayStatics::DoesSaveGameExist(TEXT("MySave"), 0);
}

void UMyGameInstance::ChangeLevel(TSoftObjectPtr<UWorld> Level)
{
	if (mHasSave)
	{

	}
	else
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), Level);
}
