// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Skill_Muriel.h"

UGA_Skill_Muriel::UGA_Skill_Muriel()
{
	static ConstructorHelpers::FClassFinder<UCameraShakeBase>
		CSB(TEXT("/Script/Engine.Blueprint'/Game/CameraShake/BS_Muriel_Skill_CameraShake.BS_Muriel_Skill_CameraShake_C'"));
	if (CSB.Succeeded())
		mCameraShake = CSB.Class;

	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		PS(TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMuriel/FX/Particles/Abilities/Primary/FX/P_Muriel_Primary_HitWorld.P_Muriel_Primary_HitWorld'"));
	if (PS.Succeeded())
		mHitEffect = PS.Object;

	mSkillSB = CreateDefaultSubobject<UAudioComponent>(TEXT("SkillSB"));

	static ConstructorHelpers::FObjectFinder<USoundBase>
		SkillSB(TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Muriel_Skill.SW_Muriel_Skill'"));
	if (SkillSB.Succeeded())
		mSkillSB->SetSound(SkillSB.Object);

	mSkillFireSB = CreateDefaultSubobject<UAudioComponent>(TEXT("SkillFireSB"));

	static ConstructorHelpers::FObjectFinder<USoundBase>
		SkillFireSB(TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Muriel_SkillFire.SW_Muriel_SkillFire'"));
	if (SkillFireSB.Succeeded())
		mSkillFireSB->SetSound(SkillFireSB.Object);

}

void UGA_Skill_Muriel::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	mSpecHandle = Handle;
	mActorInfo = ActorInfo;
	mActivationInfo = ActivationInfo;
	mMuriel = Cast<AMuriel>(mActorInfo->AvatarActor);

	UAbilityTask_WaitDelay* AT_WaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, 1);
	if (IsValid(AT_WaitDelay))
	{
		AT_WaitDelay->OnFinish.AddDynamic(this, &UGA_Skill_Muriel::SkillStart);
		AT_WaitDelay->Activate();
	}

	if(IsValid(mMuriel))
		mMuriel->SkillEffectOn(0);

	mSkillSB->Play();
}

void UGA_Skill_Muriel::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if(IsValid(mMuriel))
		mMuriel->SkillEffectOn(2);

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StopAllCameraShakes();

	mSkillSB->Stop();
	mSkillFireSB->Stop();
}

void UGA_Skill_Muriel::SkillStart()
{
	if (!IsValid(mMuriel))
	{
		Log(TEXT("Muriel Is Invalid"));
		return;
	}

	mMuriel->SkillEffectOn(1);

	if(mCameraShake)
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), mCameraShake, mMuriel->GetActorLocation(), 0, 10000);

	GetWorld()->GetTimerManager().SetTimer(mSkillTimer, this, &UGA_Skill_Muriel::SkillFire, 0.1, true);

	mSkillSB->Stop();
	mSkillFireSB->Play();

}

void UGA_Skill_Muriel::SkillFire()
{
	if (!IsValid(mMuriel))
	{
		Log(TEXT("Muriel Is Invalid"));
		EndAbility(mSpecHandle, mActorInfo, mActivationInfo, false, true);
		GetWorld()->GetTimerManager().ClearTimer(mSkillTimer);

		return;
	}

	if (!CommitAbility(mSpecHandle, mActorInfo, mActivationInfo))
	{
		EndAbility(mSpecHandle, mActorInfo, mActivationInfo, false, true);
		GetWorld()->GetTimerManager().ClearTimer(mSkillTimer);

		mMuriel->SkillEffectOn(2);
		return;
	}

	TArray<FHitResult> Results;
	FVector Pos = mMuriel->GetActorLocation() + mMuriel->GetDirSceneVector() * 10000 + FVector(0, 0, 50);
	bool Collision = GetWorld()->SweepMultiByProfile(Results, mMuriel->GetActorLocation() + FVector(0, 0, 50), Pos, FQuat::Identity,
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(50));
	
	if (Collision)
	{
		if (IsValid(mHitEffect))
		{
			ACharacterState* CS = mMuriel->GetPlayerState<ACharacterState>();
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				FVector Pos2 = Results[i].GetActor()->GetActorLocation();
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mHitEffect, Pos2, FRotator::ZeroRotator, FVector(2));
	
				AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
				if (IsValid(CS) && IsValid(Monster))
					CS->PlayGE_Skill(TEXT("Muriel"), Monster->GetAbilitySystemComponent());
			}
		}

		USoundBase* SB = LoadObject<USoundBase>(GetWorld(), TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Muriel_Skill_Hit.SW_Muriel_Skill_Hit'"));
		if (IsValid(SB))
			UGameplayStatics::PlaySound2D(GetWorld(), SB);
	}
}
