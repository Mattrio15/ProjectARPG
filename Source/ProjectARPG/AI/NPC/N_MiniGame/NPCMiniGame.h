// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../NPCBase.h"
#include "NPCMiniGame.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API ANPCMiniGame : public ANPCBase
{
	GENERATED_BODY()

public:
	ANPCMiniGame();

protected:
	virtual void NPCBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void NPCEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
