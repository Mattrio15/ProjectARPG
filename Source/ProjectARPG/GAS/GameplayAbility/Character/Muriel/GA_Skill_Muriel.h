// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../GA_Info.h"
#include "../../../../Character/CharacterState.h"
#include "../../../../Character/C_Muriel/Muriel.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Skill_Muriel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UGA_Skill_Muriel : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	FGameplayAbilitySpecHandle mSpecHandle;
	const FGameplayAbilityActorInfo* mActorInfo;
	FGameplayAbilityActivationInfo mActivationInfo;

	AMuriel* mMuriel;

	FTimerHandle mSkillTimer;

	TSubclassOf<UCameraShakeBase> mCameraShake;
	UParticleSystem* mHitEffect;

	TObjectPtr<UAudioComponent> mSkillSB;
	TObjectPtr<UAudioComponent> mSkillFireSB;

public:
	UGA_Skill_Muriel();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

public:
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility);

protected:
	UFUNCTION()
	void SkillStart();

	UFUNCTION()
	void SkillFire();

};
