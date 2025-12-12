// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../CharacterBase.h"
#include "Terra.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API ATerra : public ACharacterBase
{
	GENERATED_BODY()

public:
	ATerra();

protected:
	bool mSkilling = false;
	FTimerHandle mMoveEnableTimer;

	bool mUltimating = false;
	FVector mLocation;
	FTimerHandle mUltimateTimer;
	FTimerHandle mUltimateDilationTimer;
	float mArmLen = 0;
	int32 mUltimateEffectCount = 0;

	TSubclassOf<UGameplayAbility> mGA_Skill_Terra;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void DisappearAnimation();

	virtual void ShortAttack();
	virtual void ShortSkill();
	virtual void Ultimate();
	virtual void UltimateEffect();
	virtual void DodgeAttack();
	
	void MoveEnableTimer();
	void UltimateEnd();
	void UltimateDilationEnd();

public:
	virtual void RealAttack(int32 Index);
	virtual void RealSkill();
	virtual void RealDodgeAttack();
	virtual void RealUltimate();

};
