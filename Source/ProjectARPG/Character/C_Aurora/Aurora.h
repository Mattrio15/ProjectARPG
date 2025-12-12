// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../CharacterBase.h"
#include "Aurora.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API AAurora : public ACharacterBase
{
	GENERATED_BODY()

public:
	AAurora();

protected:
	FTimerHandle mUltimateTimer;
	bool mUltimating = false;
	FVector mLocation;
	float mArmLength = 0;
	float mUTimer = 0;
	int32 mUCount = 0;
	TArray<int32> mUArray;

	UNiagaraComponent* mUltimateEffect;
	TSubclassOf<UGameplayAbility> mGA_Skill_Aurora;
	USoundBase* mSB_Attack;
	USoundBase* mSB_Skill;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void DisappearAnimation();

	virtual void ShortAttack();
	virtual void ShortSkill();
	virtual void Ultimate();
	virtual void DodgeAttack();

public:
	virtual void ShortAttackEffect(int32 Index, float Yaw);
	virtual void ShortSkillEffect(int32 Index, float Yaw);

	virtual void RealAttack(int32 Index);
	virtual void RealSkill();
	virtual void RealDodgeAttack();
	virtual void RealUltimate();

protected:
	void FinishUltimateDilation();
		
};
