// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "Animation/AnimInstance.h"
#include "AnimInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mDirYaw = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool mIsDodge = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool mMovingDodge = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* mAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* mSkillMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* mUltimateMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* mCounterMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool mAttackReady = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool mStopMoveDodge = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool mIsKnockback = false;

	int32 mAttackIndex = 0;
	bool mAttackEnable = true;
	bool mCombo = false;
	bool mMoving = false;
	float mMovingTimer = 0;

	FVector mDirVector = FVector(0, 0, 0);
	float mForward = 0;

	float mCounterYaw = 0;

	bool mSkillEnable = true;
	int32 mSkillIndex = 0;

public:
	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	virtual void NativeBeginPlay();

public:
	float GetDirYaw() { return mDirYaw; }

public:
	void SetDirYaw(float DirYaw) { mDirYaw = DirYaw; }
	void SetDodge(bool A) { mIsDodge = A; }
	void SetMovingDodge(bool A) { mMovingDodge = A; }
	void SetAttackEnable(bool A) { mAttackEnable = A; }
	void SetSkillEnable(bool A) { mSkillEnable = A; }
	void SetAttackReady(bool A) { mAttackReady = A; }
	void SetStopMoveDodge(bool A) { mStopMoveDodge = A; }
	void SetIsKnockback(bool A);

public:
	void PlayShortAttack();
	void PlayLongAttack();
	void PlayDodgeAndCounterAttack(int32 Index);
	void PlayShortSkill(int32 Index);
	void PlayLongSkill(int32 Index);
	void PlayUltimate(int32 Index);

	void StopAttackMontage();

protected:
	UFUNCTION()
	void AnimNotify_Combo();
	UFUNCTION()
	void AnimNotify_SkillEnd();
	UFUNCTION()
	void AnimNotify_Effect_Attack();
	UFUNCTION()
	void AnimNotify_Effect_Skill();
	UFUNCTION()
	void AnimNotify_Effect_Ultimate();
	UFUNCTION()
	void AnimNotify_DodgeAttackEnd();
	UFUNCTION()
	void AnimNotify_FinishDilation();
	UFUNCTION()
	void AnimNotify_RealAttack();
	UFUNCTION()
	void AnimNotify_RealSkill();
	UFUNCTION()
	void AnimNotify_RealDodgeAttack();
	UFUNCTION()
	void AnimNotify_RealUltimate();

protected:
	UFUNCTION()
	void OnMotageEnd(UAnimMontage* Montage, bool bInterrupted);

};
