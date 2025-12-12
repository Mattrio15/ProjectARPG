// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../GA_Info.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Skill_Aurora.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UGA_Skill_Aurora : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

};
