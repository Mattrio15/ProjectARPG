// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Character/CharacterState.h"
#include "SaveGame/MySaveGame.h"

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
		LoadGame();
	}
	else
	{

	}

	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), Level);
}

void UMyGameInstance::SaveGame()
{
	ACharacterState* CS = GetFirstLocalPlayerController()->GetPlayerState<ACharacterState>();
	if (!IsValid(CS))
	{
		Test(TEXT("CS is Invalid!"));
		return;
	}

	UMySaveGame* SaveObject = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	SaveObject->SetSaveGameData(CS->GetSaveGameData());

	FAsyncSaveGameToSlotDelegate SaveDelegate;
	SaveDelegate.BindUObject(this, &UMyGameInstance::OnAsyncSaveFinished);

	UGameplayStatics::AsyncSaveGameToSlot(SaveObject, TEXT("MySave"), 0, SaveDelegate);

}

void UMyGameInstance::LoadGame()
{
	FAsyncLoadGameFromSlotDelegate LoadDelegate;
	LoadDelegate.BindUObject(this, &UMyGameInstance::OnAsyncLoadFinished);

	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("MySave"), 0, LoadDelegate);

}

void UMyGameInstance::OnAsyncSaveFinished(const FString& SlotName, int32 UserIndex, bool bSuccess)
{
	mHasSave = bSuccess;

	OnSaveFinished.Broadcast(bSuccess);

}

void UMyGameInstance::OnAsyncLoadFinished(const FString& SlotName, int32 UserIndex, USaveGame* LoadedGame)
{
	UMySaveGame* LoadObject = Cast<UMySaveGame>(LoadedGame);

	if (IsValid(LoadObject))
	{
		FSaveGameData SGD = LoadObject->GetSaveGameData();

		OnLoadFinished.Broadcast(true);
	}
	else
	{
		Log(TEXT("Load Failed!"));
		Test(TEXT("Load Failed!"));

		OnLoadFinished.Broadcast(false);
	}

}
