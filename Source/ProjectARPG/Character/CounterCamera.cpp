// Fill out your copyright notice in the Description page of Project Settings.


#include "CounterCamera.h"

ACounterCamera::ACounterCamera()
{
	mRoot = MyObject(USceneComponent, "Root");
	mArm = MyObject(USpringArmComponent, "Arm");
	mCamera = MyObject(UCameraComponent, "Camera");

	SetRootComponent(mRoot);
	mArm->SetupAttachment(mRoot);
	mCamera->SetupAttachment(mArm);

	mArm->TargetArmLength = 300;

	mPost = MyObject(UPostProcessComponent, "Post");
	mPost->SetupAttachment(mRoot);
	mPost->Settings.SceneFringeIntensity = 5;
	mPost->Settings.VignetteIntensity = 1;
}

void ACounterCamera::PostOff()
{
	mPost->Settings.bOverride_SceneFringeIntensity = false;
	mPost->Settings.bOverride_VignetteIntensity = false;
}

void ACounterCamera::PostOn(bool A)
{
	mPost->Settings.bOverride_SceneFringeIntensity = A;
	mPost->Settings.bOverride_VignetteIntensity = A;

	GetWorld()->GetTimerManager().SetTimer(mPostTimer, this, &ACounterCamera::PostOff, 1);
}

