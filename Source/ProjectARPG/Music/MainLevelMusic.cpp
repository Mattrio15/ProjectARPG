// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLevelMusic.h"
#include "../MyGameInstance.h"
#include "../Character/CharacterState.h"

AMainLevelMusic::AMainLevelMusic()
{
	mRoot = MyObject(USceneComponent, "Root");
	SetRootComponent(mRoot);
	mRoot->bVisualizeComponent = true;

	mBGM = MyObject(UAudioComponent, "BGM");
	mBGM->SetupAttachment(mRoot);

	mBGM->bAutoActivate = false;

}

void AMainLevelMusic::BeginPlay()
{
	Super::BeginPlay();
	
	UMyGameInstance* GI = GetGameInstance<UMyGameInstance>();
	if (!IsValid(GI))
		return;
	
	bool A = GI->GetIsNewGame();
	if (A)
	{
		// PlayLevelSequence();
	}
	else
		PlayBGM();

}

void AMainLevelMusic::PlayLevelSequence()
{
	if (!IsValid(mStartSequence))
		return;

	FMovieSceneSequencePlaybackSettings Setting;
	Setting.bHideHud = true;
	ALevelSequenceActor* LSA = nullptr;
	ULevelSequencePlayer* LSP = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), mStartSequence, Setting, LSA);
	if (!IsValid(LSP))
		return;
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PC))
		return;
	ACharacterState* CS = PC->GetPlayerState<ACharacterState>();
	if (!IsValid(CS))
		return;

	FInputModeUIOnly Mode;
	PC->SetInputMode(Mode);
	PC->SetShowMouseCursor(false);
	CS->ShowMainWidget(false);
	float EndTime = LSP->GetEndTime().AsSeconds();
	LSP->Play();

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,
		[PC, CS, this]()
		{
			FInputModeGameOnly Mode;
			PC->SetInputMode(Mode);
			CS->ShowMainWidget(true);
			this->PlayBGM();
		},
		EndTime, false
	);

}

void AMainLevelMusic::PlayBGM()
{
	if (!IsValid(mBGM->GetSound()))
		return;

	mBGM->Play();
	mBGM->FadeIn(5);

}
