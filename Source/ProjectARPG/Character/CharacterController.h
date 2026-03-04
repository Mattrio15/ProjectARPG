// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "CounterCamera.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

class ACharacterBase;

UCLASS()
class PROJECTARPG_API ACharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	ACharacterController();

protected:
	int32 mCharacterIndex = 0;
	bool mCounterEnable = false;

	ACounterCamera* mCounterCamera;

	FTimerHandle mSpawnTimer;
	
	UPROPERTY()
	TSoftObjectPtr<UNiagaraSystem> mCounterAttackEffect; // 캐릭터 베이스에서 넘어옴

protected:
	virtual void BeginPlay() override;

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	void SpawnCharacter();

public:
	void CharacterChange(FVector Postion, FRotator CameraRotation, float DirYaw);
	void SetCounterEnable(bool A) { mCounterEnable = A; }

	bool GetCounterEnable() { return  mCounterEnable; }

};
