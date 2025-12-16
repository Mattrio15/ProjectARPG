// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Dodge.h"
#include "../../../Character/CharacterBase.h"
#include "../../../Character/AnimInstanceBase.h"

void UGA_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacterBase* Character = Cast<ACharacterBase>(ActorInfo->AvatarActor);
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(ActorInfo->GetAnimInstance());

	if (!IsValid(Character) || !IsValid(AIB))
	{
		Test(TEXT("Character or Anim Is Invalid!"));
		Log(TEXT("Character or Anim Is Invalid!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	FVector DirVec = Character->GetVelocity();
	DirVec.Normalize();
	Test(DirVec.ToString());

	bool Moving = ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Moving")));
	if (Moving)
	{
		AIB->SetDirYaw(DirVec.Rotation().Yaw);
	}
	else
	{
		AIB->SetDirYaw(Character->GetDirYaw());
	}



	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
