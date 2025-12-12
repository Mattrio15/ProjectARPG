// Fill out your copyright notice in the Description page of Project Settings.


#include "Revenant_Ultimate.h"

ARevenant_Ultimate::ARevenant_Ultimate()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	mArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SetRootComponent(mRoot);
	mArm->SetupAttachment(mRoot);
	mCamera->SetupAttachment(mArm);

	mArm->TargetArmLength = 3000;
	mArm->SetRelativeRotation(FRotator(-45, 0, 0));
}

void ARevenant_Ultimate::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARevenant_Ultimate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

