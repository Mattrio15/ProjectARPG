// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Game_Info.h"
#include "GameFramework/Actor.h"
#include "Revenant_Ultimate.generated.h"

UCLASS()
class PROJECTARPG_API ARevenant_Ultimate : public AActor
{
	GENERATED_BODY()
	
public:	
	ARevenant_Ultimate();

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* mRoot;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
