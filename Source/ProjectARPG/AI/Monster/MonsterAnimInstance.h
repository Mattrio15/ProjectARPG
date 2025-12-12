// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../AI_Info.h"
#include "Animation/AnimInstance.h"
#include "MonsterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* mAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool mIsCriticalHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool mIsHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool mIsDeath = false;

public:
	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	virtual void NativeBeginPlay();

public:
	void PlayMonsterAttack();

	void SetIsCriticalHit(bool A) { mIsCriticalHit = A; }
	void SetIsHit(bool A) { mIsHit = A; }
	void SetIsDeath(bool A) { mIsDeath = A; }

protected:
	UFUNCTION()
	void AnimNotify_MonsterAttack();
	UFUNCTION()
	void AnimNotify_CriticalHitEnd();
	UFUNCTION()
	void AnimNotify_RealAttack();

protected:
	UFUNCTION()
	void MontageEnd(UAnimMontage* Montage, bool bInterrupted);

};
