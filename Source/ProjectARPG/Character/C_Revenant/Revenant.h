// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../CharacterBase.h"
#include "Revenant.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API ARevenant : public ACharacterBase
{
	GENERATED_BODY()

public:
	ARevenant();

protected:
	UNiagaraComponent* mSkillEffect;
	FTimerHandle mSkillTimer;
	bool mSkilling = false;
	float mSkillDirTimer = 0;
	FVector mSkillDir;
	int32 mSkillDirPM = 1;
	float mArmLen = 0;
	TArray<FVector> mSkillVecArray;
	TArray<FRotator> mSkillRotArray;

	bool mUltimating = false;

	AActor* mUltimateActor;
	FTimerHandle mUltimateTimer;
	int32 mUltimateCount = 0;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void DisappearAnimation();

	virtual void ShortAttack();
	virtual void LongAttack();
	virtual void ShortSkill();
	virtual void Ultimate();

public:
	virtual void ShortAttackEffect(int32 Index, float Yaw);

	virtual void RealAttack(int32 Index);
	virtual void RealSkill();

protected:
	void SkillEnd();
	void UltimateEnd();

};
