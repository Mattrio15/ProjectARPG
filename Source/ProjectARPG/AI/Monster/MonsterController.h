// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../AI_Info.h"
#include "AIController.h"
#include "MonsterController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API AMonsterController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterController();

protected:
	UAIPerceptionComponent* mPerception;
	UAISenseConfig_Sight* mAISenseSight;
	TArray<AActor*> mTargets;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

public:
	void SetPD();

protected:
	UFUNCTION()
	void TargetUpdate(AActor* Actor, FAIStimulus Stimulus);
	
};
