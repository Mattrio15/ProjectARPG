// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "InputAction.h"
#include "AnimInstanceBase.h"
#include "CharacterState.h"
#include "../AI/Monster/MonsterBase.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

UCLASS()
class PROJECTARPG_API ACharacterBase : public ACharacter,public IGenericTeamAgentInterface // public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();

protected:
	FGenericTeamId mTeamId = FGenericTeamId(1);

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;
	USceneComponent* mDirScene;

	bool mMoveEnable = true;
	bool mCameraRotationEnable = true;

	FTimerHandle mChangeTimer;
	bool mDisappear = false;
	FVector mDisappearDir;

	bool mForward = false;
	float mForwardTimer = 0;

	bool mDodge = false;
	float mDodgeTimer = 0;
	FVector mDodgeDir;
	bool mDodgeEnable = true;

	FVector mDirVector = FVector(0, 0, 0);
	bool mMoving = false;
	FVector mDodgeDirVector = FVector(0, 0, 0);

	bool mPDEnable = false;
	FTimerHandle mDilationTimer;
	FTimerHandle mPDFail;

	UNiagaraComponent* mDodgeAfterImage;
	UPROPERTY(VisibleAnywhere)
	UPostProcessComponent* mDodgePostProcess;
	bool mDPPEnable = false;

	bool mAttackEnable = true;

	bool mSkillEnable = true;

	bool mUltimateDilation = false;
	bool mUltimateEnable = true;

	TArray<AActor*> mTargetArray;
	AActor* mTarget;

	FName mName;

	bool mShowUI = false;

public:
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) {}
	virtual FGenericTeamId GetGenericTeamId() const { return mTeamId; }
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	void CharacterMoveStart(const FInputActionInstance& Instance);
	void CharacterMove(const FInputActionInstance& Instance);
	void CharacterMoveOff(const FInputActionInstance& Instance);
	void CharacterCameraRotation(const FInputActionInstance& Instance);
	void CharacterChange(const FInputActionInstance& Instance);
	void CharacterDodge(const FInputActionInstance& Instance);
	void CharacterAttack(const FInputActionInstance& Instance);
	void CharacterAttackTriggered(const FInputActionInstance& Instance);
	void CharacterAttackCompleted(const FInputActionInstance& Instance);
	void CharacterSkill(const FInputActionInstance& Instance);
	void CharacterSkillTriggered(const FInputActionInstance& Instance);
	void CharacterSkillCompleted(const FInputActionInstance& Instance);
	void CharacterUltimate(const FInputActionInstance& Instance);
	void CharacterInventory(const FInputActionInstance& Instance);
	void CharacterQuickSlot_1(const FInputActionInstance& Instance);
	void CharacterQuickSlot_2(const FInputActionInstance& Instance);
	void CharacterQuickSlot_3(const FInputActionInstance& Instance);



protected:
	void CharacterDisappear();
	virtual void DisappearAnimation() {}
	void FinishDilation();
	void PerfectDodgeFail() { mPDEnable = false; }

	virtual void ShortAttack(){}
	virtual void LongAttack(){}
	virtual void MoveAttack(){}
	virtual void ShortSkill(){}
	virtual void LongSkillReady(){}
	virtual void LongSkillTriggered(){}
	virtual void LongSkill(){}
	virtual void LongSkillCompleted(){}
	virtual void Ultimate(){}
	virtual void DodgeAttack();

public:
	virtual void ShortAttackEffect(int32 Index, float Yaw){}
	virtual void ShortSkillEffect(int32 Index, float Yaw) {}
	virtual void LongSkillEffect(int32 Index, float Yaw){}
	virtual void UltimateEffect(){}
	virtual void RealAttack(int32 Index){}
	virtual void RealSkill(){}
	virtual void RealDodgeAttack(){}
	virtual void RealUltimate(){}

public:
	void SetCameraRotation(FRotator CameraRotation);
	void SetDirYaw(float DirYaw);
	void SetMoveEnable(bool A) { mMoveEnable = A; }
	void SetDodgeEnable(bool A) { mDodgeEnable = A; }
	void SetAttackEnable(bool A) { mAttackEnable = A; }
	void SetSkillEnable(bool A) { mSkillEnable = A; }
	void SetTargeting(AActor* Target);
	void SetPDEnable(AActor* Target);

	void ShowUI(bool A);

	FVector GetDirVector() { return mDirVector; }
	float GetDirYaw() { return mDirScene->GetRelativeRotation().Yaw; }
	FName GetCharacterName() { return mName; }

public:
	void CounterChange();

	void GiveDamage(AActor* Target, float Damage, bool Critical);

	virtual void LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride);


protected:
	UFUNCTION()
	void PDEnd() { mPDEnable = false; }

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);


};
