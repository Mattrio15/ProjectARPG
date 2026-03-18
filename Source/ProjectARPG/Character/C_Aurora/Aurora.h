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
	FTimerHandle mUltimateTimer; // 궁극기 타이머
	bool mUltimating = false; // 궁극기 사용 중 여부
	FVector mLocation; // 궁극기 사용 시 위치
	float mArmLength = 0; // 스프링 암 길이
	float mUTimer = 0; // 궁극기 내부 타이머
	int32 mUCount = 0; // 궁극기 카운트
	TArray<int32> mUArray; // 궁극기 배열

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> mNC_Ultimate; // 궁극기 용 나이아가라 컴포넌트
	UPROPERTY()
	TSubclassOf<UGameplayAbility> mGA_Skill_Aurora; // 스킬 용 GA
	UPROPERTY()
	TObjectPtr<USoundBase> mSB_Attack; // 공격 소리
	UPROPERTY()
	TObjectPtr<USoundBase> mSB_AttackHit; // 공격 타격 소리
	UPROPERTY()
	TObjectPtr<USoundBase> mSB_Skill; // 스킬 소리
	UPROPERTY()
	TObjectPtr<USoundBase> mSB_SkillHit; // 스킬 타격 소리

	UPROPERTY()
	TObjectPtr<UParticleSystem> mPS_Ultimate; // 궁극기 파티클

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> mNS_Attack; // 공격 용 나이아가라 시스템
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> mNS_AttackHit; // 공격 타격 용 나이아가라 시스템
	UPROPERTY()
	TObjectPtr<UParticleSystem> mPS_Skill; // 스킬 용 파티클 시스템
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> mNS_Skill; // 스킬 용 나이아가라 시스템
	UPROPERTY()
	TObjectPtr<UParticleSystem> mPS_SkillHit; // 스킬 타격 용 파티클 시스템


public:
	virtual void Tick(float DeltaTime) override;

protected:
	// virtual void DisappearAnimation(); // 퇴장 애니메이션
	
	virtual void ShortSkill(); // 짧은 스킬
	virtual void Ultimate(); // 궁극기 
	virtual void DodgeAttack(); // 회피 공격

public:
	virtual void ShortAttackEffect(int32 Index, float Yaw); // 공격 이펙트
	virtual void ShortSkillEffect(int32 Index, float Yaw); // 스킬 이펙트

	virtual void RealAttack(int32 Index); // 실제 공격
	virtual void RealSkill(); // 실제 스킬
	virtual void RealDodgeAttack(); // 실제 회피 공격
	virtual void RealUltimate(); // 실제 궁극기

protected:
	void FinishUltimateDilation(); // 궁극기 종료 시 슬로우 모션 종료
		
};
