// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GA_Info.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Dodge.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UGA_Dodge : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	FTimerHandle mCameraTimer;
	UPostProcessComponent* mDPP;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

protected:
	UFUNCTION()
	void FinishDodge();

	void SetCameraEffect();
	
};
