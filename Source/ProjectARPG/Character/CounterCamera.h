// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "GameFramework/Actor.h"
#include "CounterCamera.generated.h"

UCLASS()
class PROJECTARPG_API ACounterCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	ACounterCamera();

protected:
	USceneComponent* mRoot;
	USpringArmComponent* mArm;
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	UPostProcessComponent* mPost;

	FTimerHandle mPostTimer;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	void PostOff();

public:
	void PostOn(bool A);

};
