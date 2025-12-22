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


	bool Moving = ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Moving")));
	if (Moving)
		AIB->SetDirYaw(DirVec.Rotation().Yaw);
	else
	{
		AIB->SetDirYaw(Character->GetDirYaw());
		FRotator Rot = FRotator(0, Character->GetDirYaw(), 0);
		DirVec = Rot.Vector() * (-1);
	}

	AIB->StopAttackMontage();
	AIB->StopAllMontages(0.1);
	AIB->SetDodge(true);

	UAbilityTask_ApplyRootMotionConstantForce* AT_CF = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this, TEXT("Dodge"), DirVec, 1000, 0.3, true, nullptr, ERootMotionFinishVelocityMode::SetVelocity, FVector(0, 0, 0), 0, true);
	
	AT_CF->OnFinish.AddDynamic(this, &UGA_Dodge::FinishDodge);
	AT_CF->ReadyForActivation();


	if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.PDEnable"))))
	{
		ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.NoDamage")));
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.25);
		mDPP = Character->GetDPP();
		if (IsValid(mDPP))
		{
			mDPP->Settings.bOverride_SceneFringeIntensity = true;
			mDPP->Settings.bOverride_VignetteIntensity = true;
			SetCameraEffect();

			UNiagaraComponent* DAI = Character->GetDAI();
			if (IsValid(DAI))
				DAI->Activate(false);
		}

		USoundBase* SB = LoadObject<USoundBase>(GetWorld(), TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_PerfectDodge.SW_PerfectDodge'"));
		if (IsValid(SB))
			UGameplayStatics::PlaySound2D(GetWorld(), SB);
	}

}

void UGA_Dodge::FinishDodge()
{
	CurrentActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.NoDamage")));
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(CurrentActorInfo->GetAnimInstance());
	if (IsValid(AIB))
		AIB->SetDodge(false);

	ACharacterBase* Character = Cast<ACharacterBase>(CurrentActorInfo->AvatarActor);
	if (IsValid(Character))
	{
		Character->SetMoveEnable(true);
		Character->SetAttackEnable(true);
		Character->SetSkillEnable(true);
	}

	if (IsValid(mDPP))
	{
		mDPP->Settings.SceneFringeIntensity = 0;
		mDPP->Settings.VignetteIntensity = 0;
		mDPP->Settings.bOverride_SceneFringeIntensity = false;
		mDPP->Settings.bOverride_VignetteIntensity = false;
	}

	GetWorld()->GetTimerManager().ClearTimer(mCameraTimer);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_Dodge::SetCameraEffect()
{
	float Rate = GetWorld()->GetDeltaSeconds();

	if (mDPP->Settings.VignetteIntensity < 1)
	{
		mDPP->Settings.SceneFringeIntensity += Rate * 50;
		mDPP->Settings.VignetteIntensity += Rate * 10;
		GetWorld()->GetTimerManager().SetTimer(mCameraTimer, this, &UGA_Dodge::SetCameraEffect, Rate, false);
	}
}
