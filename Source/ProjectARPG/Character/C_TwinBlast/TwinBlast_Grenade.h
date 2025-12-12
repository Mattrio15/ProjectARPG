// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Game_Info.h"
#include "TwinBlast.h"
#include "GameFramework/Actor.h"
#include "TwinBlast_Grenade.generated.h"

UCLASS()
class PROJECTARPG_API ATwinBlast_Grenade : public AActor
{
	GENERATED_BODY()
	
public:	
	ATwinBlast_Grenade();

protected:
	USphereComponent* mRoot;
	UStaticMeshComponent* mMesh;
	UProjectileMovementComponent* mMovement;
	UNiagaraComponent* mEffect;
	int32 mIndex = 0;

	TObjectPtr<ATwinBlast> mParent;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void SetIndex(int32 Index) { mIndex = Index; }
	void SetParent(ATwinBlast* Parent) { mParent = Parent; }

protected:
	UFUNCTION()
	void GrenadeBomb(const FHitResult& ImpactResult);

};
