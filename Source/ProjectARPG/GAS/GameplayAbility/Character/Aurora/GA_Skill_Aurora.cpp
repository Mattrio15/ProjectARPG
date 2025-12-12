// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Skill_Aurora.h"
#include "../../../../Character/AnimInstanceBase.h"
#include "../../../../Character/CharacterState.h"
#include "../../../../Character/C_Aurora/Aurora.h"

void UGA_Skill_Aurora::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AAurora* Aurora = Cast<AAurora>(ActorInfo->AvatarActor);
	ACharacterState* CS = Cast<ACharacterState>(ActorInfo->OwnerActor);
	UAnimInstanceBase* Anim = Cast<UAnimInstanceBase>(ActorInfo->GetAnimInstance());

	if (!IsValid(Aurora) || !IsValid(CS) || !IsValid(Anim))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		Log(TEXT("Aurora or CS or Anim Is Invalid"));
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	Anim->PlayShortSkill(0);

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
