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
	UPROPERTY()
	TObjectPtr<USceneComponent> mRoot; // 루트
	UPROPERTY()
	TObjectPtr<USpringArmComponent> mArm; // 스프링 암
	UPROPERTY()
	TObjectPtr<UCameraComponent> mCamera; // 카메라

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPostProcessComponent> mPost; // 포스트 프로세스 컴포넌트

	FTimerHandle mPostTimer; // 포스트 프로세스 타이머

protected:
	void PostOff();

public:
	void PostOn(bool A);

};
