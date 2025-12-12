// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../CharacterBase.h"
#include "TwinBlast.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API ATwinBlast : public ACharacterBase
{
	GENERATED_BODY()

public:
	ATwinBlast();

protected:
	bool mSkilling = false;
	FTimerHandle mSkillTimer;

	UNiagaraComponent* mSkillEffect;


	float mArmLength = 0;
	FVector mCameraPos = FVector(0, 0, 0);

	bool mUltimating = false;
	FTimerHandle mUltimateTimer;

	TSubclassOf<UGameplayAbility> mGA_Skill_TwinBlast;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void DisappearAnimation();

	virtual void LongAttack();
	virtual void LongSkillReady();
	virtual void LongSkillCompleted();
	virtual void Ultimate();

protected:
	void SkillFireEnd();
	void UltimateEnd();

public:
	virtual void ShortAttackEffect(int32 Index, float Yaw);
	virtual void LongSkillEffect(int32 Index, float Yaw);
	virtual void UltimateEffect();

	virtual void RealAttack(int32 Index);
	virtual void RealSkill();
	

};
