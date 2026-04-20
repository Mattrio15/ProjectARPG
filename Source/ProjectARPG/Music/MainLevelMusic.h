// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "GameFramework/Actor.h"
#include "MainLevelMusic.generated.h"

UCLASS()
class PROJECTARPG_API AMainLevelMusic : public AActor
{
	GENERATED_BODY()
	
public:	
	AMainLevelMusic();

protected:
	UPROPERTY()
	TObjectPtr<USceneComponent> mRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> mBGM;

	UPROPERTY(EditAnywhere)
	TObjectPtr<ULevelSequence> mStartSequence;

protected:
	virtual void BeginPlay() override;

protected:
	void PlayLevelSequence();
	void PlayBGM();


};
