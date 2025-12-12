// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAnimInstance.h"
#include "MonsterController.h"
#include "MonsterBase.h"

void UMonsterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UMonsterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OnMontageEnded.AddDynamic(this, &UMonsterAnimInstance::MontageEnd);
}

void UMonsterAnimInstance::PlayMonsterAttack()
{
	if (!IsValid(mAttackMontage))
		return;

	if (Montage_IsPlaying(mAttackMontage))
		return;

	int32 Index = FMath::RandRange(0, mAttackMontage->GetNumSections()-1);
	Montage_Play(mAttackMontage);
	Montage_JumpToSection(mAttackMontage->GetSectionName(Index), mAttackMontage);

}

void UMonsterAnimInstance::AnimNotify_MonsterAttack()
{
	AMonsterController* MC = TryGetPawnOwner()->GetController<AMonsterController>();
	if (IsValid(MC))
		MC->SetPD();
}

void UMonsterAnimInstance::AnimNotify_CriticalHitEnd()
{
	AMonsterController* MC = TryGetPawnOwner()->GetController<AMonsterController>();
	if (IsValid(MC))
		MC->GetBlackboardComponent()->SetValueAsBool(TEXT("CriticalHit"), false);
	mIsCriticalHit = false;
}

void UMonsterAnimInstance::AnimNotify_RealAttack()
{
	AMonsterBase* MB = Cast<AMonsterBase>(TryGetPawnOwner());
	if (IsValid(MB))
		MB->RealAttack();
}

void UMonsterAnimInstance::MontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		AMonsterController* MC = TryGetPawnOwner()->GetController<AMonsterController>();
		if (IsValid(MC))
			MC->GetBlackboardComponent()->SetValueAsBool(TEXT("AttackEnd"), true);
	}

}
