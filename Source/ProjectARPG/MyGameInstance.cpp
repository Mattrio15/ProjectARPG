// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Character/CharacterState.h"

UMyGameInstance::UMyGameInstance()
{
}

void UMyGameInstance::Init()
{
	Super::Init();

	mHasSave = UGameplayStatics::DoesSaveGameExist(TEXT("MySave"), 0);

	if (mHasSave)
	{
		UMySaveGame* SaveObject = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("MySave"), 0));
		if (IsValid(SaveObject))
			mSaveData = SaveObject->GetSaveGameData();
	}
}

void UMyGameInstance::ChangeLevel(TSoftObjectPtr<UWorld> Level, bool IsNewGame)
{
	FInputModeGameOnly Mode;
	GetFirstLocalPlayerController(GetWorld())->SetInputMode(Mode);

	mIsNewGame = IsNewGame;

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

	mSaveData = SaveObject->GetSaveGameData();

	FAsyncSaveGameToSlotDelegate SaveDelegate;
	SaveDelegate.BindUObject(this, &UMyGameInstance::OnAsyncSaveFinished);

	UGameplayStatics::AsyncSaveGameToSlot(SaveObject, TEXT("MySave"), 0, SaveDelegate);

}

void UMyGameInstance::LoadGame()
{
	if (mHasSave)
	{
		FAsyncLoadGameFromSlotDelegate LoadDelegate;
		LoadDelegate.BindUObject(this, &UMyGameInstance::OnAsyncLoadFinished);

		UGameplayStatics::AsyncLoadGameFromSlot(TEXT("MySave"), 0, LoadDelegate);
	}

}

void UMyGameInstance::OnAsyncSaveFinished(const FString& SlotName, int32 UserIndex, bool bSuccess)
{
	mHasSave = bSuccess;
	
	if(OnSaveFinished.IsBound())
		OnSaveFinished.Broadcast(bSuccess);

}

void UMyGameInstance::OnAsyncLoadFinished(const FString& SlotName, int32 UserIndex, USaveGame* LoadedGame)
{
	UMySaveGame* LoadObject = Cast<UMySaveGame>(LoadedGame);

	if (IsValid(LoadObject))
		OnLoadFinished.Broadcast(true);
	else
	{
		Log(TEXT("Load Failed!"));
		Test(TEXT("Load Failed!"));

		OnLoadFinished.Broadcast(false);
	}

}
