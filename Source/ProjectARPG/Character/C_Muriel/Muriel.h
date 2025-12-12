// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../CharacterBase.h"
#include "Muriel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API AMuriel : public ACharacterBase
{
	GENERATED_BODY()

public:
	AMuriel();

protected:
	bool mAttackReady = false;
	float mAttackTimer = 0;
	bool mMoveAttackEnable = false;
	FTimerHandle mMoveAttackTimer;
	int32 mAttackCount = 0;

	USceneComponent* mSkillRoot;
	USceneComponent* mSkillC;
	bool mSkilling = false;

	UNiagaraComponent* mSkillEffectBase;
	UNiagaraComponent* mSkillEffect00;
	UNiagaraComponent* mSkillEffect01;
	UNiagaraComponent* mSkillEffect02;
	UNiagaraComponent* mSkillEffect03;
	UNiagaraComponent* mSkillEffect04;
	UNiagaraComponent* mSkillEffect05;
	UNiagaraComponent* mSkillEffect06;
	UNiagaraComponent* mSkillEffect07;
	UNiagaraComponent* mSkillEffect08;
	TArray<UNiagaraComponent*> mSkillEffectArray;

	UNiagaraComponent* mUltimateEffect01;
	UNiagaraComponent* mUltimateEffect02;

	FTimerHandle mSkillTimer;

	FTimerHandle mUltimateTimer;

	bool mUltimating = false;
	float mArmLen;
	float mWalkSpeed = 0;
	float mAcceleration = 0;
	float mVecZ = 0;

	FTimerHandle mSkillHitTimer;

	TSubclassOf<UGameplayAbility> mGA_Attack_Muriel;
	TSubclassOf<UGameplayAbility> mGA_Skill_Muriel;


public:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void DisappearAnimation();

	virtual void MoveAttack();
	virtual void LongAttack();
	virtual void LongSkillReady();
	virtual void LongSkillCompleted();
	virtual void Ultimate();
	virtual void UltimateEffect();

	void UltimateEnd();

public:
	FVector GetDirSceneVector();
	FVector GetEndVector();

public:
	void SkillEffectOn(int32 Index);

};
