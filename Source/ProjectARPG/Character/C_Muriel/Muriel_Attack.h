// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Game_Info.h"
#include "Muriel.h"
#include "GameFramework/Actor.h"
#include "Muriel_Attack.generated.h"

UCLASS()
class PROJECTARPG_API AMuriel_Attack : public AActor
{
	GENERATED_BODY()
	
public:	
	AMuriel_Attack();

protected:
	USphereComponent* mRoot;
	UProjectileMovementComponent* mMovement;

	AMuriel* mParent;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void SetParent(AMuriel* Parent) { mParent = Parent; }

protected:
	UFUNCTION()
	void AttackHit(const FHitResult& ImpactResult);

};
