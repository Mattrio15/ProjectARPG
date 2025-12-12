// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Game_Info.h"
#include "TwinBlast.h"
#include "GameFramework/Actor.h"
#include "TwinBlast_Ultimate_Actor.generated.h"

UCLASS()
class PROJECTARPG_API ATwinBlast_Ultimate_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATwinBlast_Ultimate_Actor();

protected:
	USphereComponent* mRoot;
	UStaticMeshComponent* mMesh;
	UProjectileMovementComponent* mMovement;
	UNiagaraComponent* mEffect;

	float mHeight = 0;
	float mMaxHeight = 0;

	ATwinBlast* mParent;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void SetParent(ATwinBlast* Parent) { mParent = Parent; }

};
