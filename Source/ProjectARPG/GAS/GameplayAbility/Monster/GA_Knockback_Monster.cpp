// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Knockback_Monster.h"
#include "../../../AI/Monster/MonsterBase.h"

void UGA_Knockback_Monster::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	AMonsterBase* Monster = Cast<AMonsterBase>(ActorInfo->AvatarActor);
	FVector Pos = Monster->GetActorForwardVector();
	Monster->LaunchCharacter(-Pos * 100, false, false);

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
