// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Game_Info.h"
#include "Muriel.h"
#include "GameFramework/Actor.h"
#include "Muriel_Ultimate.generated.h"

UCLASS()
class PROJECTARPG_API AMuriel_Ultimate : public AActor
{
	GENERATED_BODY()
	
public:	
	AMuriel_Ultimate();

protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* mRoot;
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* mMovement;

	FTimerHandle mUltimateHitTimer;

	int32 mUltimateCount = 0;

	AMuriel* mParent;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void SetHomingTarget(USceneComponent* Target);
	void SetSpeed(float Speed);
	void SetParent(AMuriel* Parent) { mParent = Parent; }

protected:
	UFUNCTION()
	void UltimateBomb(const FHitResult& ImpactResult);
	UFUNCTION()
	void UltimateHit();

};
