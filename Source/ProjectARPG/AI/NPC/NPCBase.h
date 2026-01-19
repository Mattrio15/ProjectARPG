// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../AI_Info.h"
#include "GameFramework/Pawn.h"
#include "NPCBase.generated.h"

UCLASS()
class PROJECTARPG_API ANPCBase : public APawn
{
	GENERATED_BODY()

public:
	ANPCBase();

protected:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* mRoot;
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* mMesh;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* mTrigger;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	virtual void NPCBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,	UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { }

	UFUNCTION()
	virtual void NPCEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){}

};
