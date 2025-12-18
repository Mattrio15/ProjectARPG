// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceBase.h"
#include "CharacterBase.h"

void UAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACharacterBase* Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (mMoving)
	{
		mMovingTimer += DeltaSeconds;
		if (IsValid(Character))
		{
			mForward += DeltaSeconds;
			float Dis = FMath::Pow(0.2, mForward - 1);
			Character->AddMovementInput(mDirVector, 1);
		}
		if (mMovingTimer > 0.5)
		{
			mMoving = false;
			mMovingTimer = 0;
		}
	}

	if (mIsKnockback)
	{
		if (IsValid(Character))
		{
			float Vel = Character->GetCharacterMovement()->Velocity.Length();
			if (Vel == 0)
			{
				mIsKnockback = false;
				// mAttackEnable = true;
				Character->SetMoveEnable(true);
				Character->SetAttackEnable(true);
				Character->SetSkillEnable(true);
			}
		}
	}

}

void UAnimInstanceBase::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OnMontageEnded.AddDynamic(this, &UAnimInstanceBase::OnMotageEnd);
}

void UAnimInstanceBase::SetIsKnockback(bool A)
{
	mIsKnockback = A;
	if (A)
	{
		ACharacterBase* CB = Cast<ACharacterBase>(TryGetPawnOwner());
		if (IsValid(CB))
		{
			CB->SetMoveEnable(false);
			CB->SetAttackEnable(false);
			CB->SetSkillEnable(false);
		}
	}
}

void UAnimInstanceBase::PlayShortAttack()
{
	if (!IsValid(mAttackMontage))
		return;

	ACharacterBase* Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (!IsValid(Character))
		return;

	Character->SetMoveEnable(false);
	Character->SetAttackEnable(false);

	mDirVector = Character->GetDirVector();
	if (mDirVector.Length() > 0)
		SetDirYaw(mDirVector.Rotation().Yaw);

	mMoving = true;
	mForward = 0;

	Montage_Play(mAttackMontage);
	Montage_JumpToSection(mAttackMontage->GetSectionName(mAttackIndex%(mAttackMontage->GetNumSections())));

	mAttackIndex += 1;
}

void UAnimInstanceBase::PlayLongAttack()
{
	if (!IsValid(mAttackMontage))
		return;

	ACharacterBase* Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (!IsValid(Character))
		return;
	Character->SetMoveEnable(false);
	Character->SetAttackEnable(false);

	// if (Character->GetDirVector().Length() > 0)
	// 	SetDirYaw(Character->GetDirVector().Rotation().Yaw);
	// else
	// 	SetDirYaw(Character->GetDirYaw());
	SetDirYaw(Character->GetDirYaw());

	Montage_Play(mAttackMontage);
	Montage_JumpToSection(mAttackMontage->GetSectionName(mAttackIndex%(mAttackMontage->GetNumSections())));

	mAttackIndex += 1;
}

void UAnimInstanceBase::PlayDodgeAndCounterAttack(int32 Index)
{
	if (!IsValid(mCounterMontage))
		return;

	if (!mAttackEnable)
		return;

	ACharacterBase* Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (!IsValid(Character))
		return;

	Montage_Play(mCounterMontage);
	Montage_JumpToSection(mCounterMontage->GetSectionName(Index));
}

void UAnimInstanceBase::PlayShortSkill(int32 Index)
{
	StopAttackMontage();
	if (!IsValid(mSkillMontage))
		return;

	if (!mSkillEnable)
		return;

	ACharacterBase* Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (!IsValid(Character))
		return;
	Character->SetMoveEnable(false);

	Montage_Play(mSkillMontage);
	Montage_JumpToSection(mSkillMontage->GetSectionName(Index));

	mSkillEnable = false;
	// mAttackEnable = false;
}

void UAnimInstanceBase::PlayLongSkill(int32 Index)
{
	if (!IsValid(mSkillMontage) || !mSkillEnable)
		return;

	if(Montage_IsPlaying(mSkillMontage))
		Montage_JumpToSection(mSkillMontage->GetSectionName(Index));
	else
		Montage_Play(mSkillMontage);

}

void UAnimInstanceBase::PlayUltimate(int32 Index)
{
	if (!IsValid(mUltimateMontage))
		return;

	StopAllMontages(0.1);

	Montage_Play(mUltimateMontage);
	Montage_JumpToSection(mUltimateMontage->GetSectionName(Index));

	mSkillEnable = false;
}

void UAnimInstanceBase::StopAttackMontage()
{
	if (Montage_IsPlaying(mAttackMontage))
	{
		Montage_Stop(0.25, mAttackMontage);
		mAttackIndex = 0;
		mMoving = false;
	}
}

void UAnimInstanceBase::AnimNotify_Combo()
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningActor());
	if (IsValid(Character))
		Character->SetAttackEnable(true);
	mCombo = true;
}

void UAnimInstanceBase::AnimNotify_SkillEnd()
{
	mSkillEnable = true;
	ACharacterBase* Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (IsValid(Character))
	{
		Character->SetMoveEnable(true);
		Character->SetDodgeEnable(true);
		Character->SetAttackEnable(true);
		Character->SetSkillEnable(true);
	}
	StopAllMontages(0.25);
	// mAttackEnable = true;
}

void UAnimInstanceBase::AnimNotify_Effect_Attack()
{
	ACharacterBase* Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (!IsValid(Character))
		return;
	int32 Index = mAttackIndex + mAttackMontage->GetNumSections() - 1;
	Index %= mAttackMontage->GetNumSections();
	Character->ShortAttackEffect(Index, mDirYaw);
}

void UAnimInstanceBase::AnimNotify_Effect_Skill()
{
	ACharacterBase* Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (!IsValid(Character))
		return;
	int32 Index = mSkillIndex + mSkillMontage->GetNumSections() - 1;
	Index %= mSkillMontage->GetNumSections();
	Character->ShortSkillEffect(Index, mDirYaw);
	Character->LongSkillEffect(Index, mDirYaw);
}

void UAnimInstanceBase::AnimNotify_Effect_Ultimate()
{
	ACharacterBase* Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (!IsValid(Character))
		return;
	Character->UltimateEffect();
}

void UAnimInstanceBase::AnimNotify_DodgeAttackEnd()
{
	ACharacterBase* Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (IsValid(Character))
	{
		Character->SetMoveEnable(true);
		// mAttackEnable = true;
	}
}

void UAnimInstanceBase::AnimNotify_FinishDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	TryGetPawnOwner()->GetController<APlayerController>()->SetViewTargetWithBlend(TryGetPawnOwner(), 0.25);
}

void UAnimInstanceBase::AnimNotify_RealAttack()
{
	ACharacterBase* CB = Cast<ACharacterBase>(TryGetPawnOwner());
	if (IsValid(CB))
		CB->RealAttack((mAttackIndex - 1) % (mAttackMontage->GetNumSections()));
}

void UAnimInstanceBase::AnimNotify_RealSkill()
{
	ACharacterBase* CB = Cast<ACharacterBase>(TryGetPawnOwner());
	if (IsValid(CB))
		CB->RealSkill();
}

void UAnimInstanceBase::AnimNotify_RealDodgeAttack()
{
	ACharacterBase* CB = Cast<ACharacterBase>(TryGetPawnOwner());
	if (IsValid(CB))
		CB->RealDodgeAttack();
}

void UAnimInstanceBase::AnimNotify_RealUltimate()
{
	ACharacterBase* CB = Cast<ACharacterBase>(TryGetPawnOwner());
	if (IsValid(CB))
		CB->RealUltimate();
}

void UAnimInstanceBase::OnMotageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		ACharacterBase* Character = Cast<ACharacterBase>(TryGetPawnOwner());
		if (IsValid(Character))
		{
			Character->SetMoveEnable(true);
			Character->SetAttackEnable(true);
			Character->SetSkillEnable(true);
		}

		// mAttackEnable = true;
		mSkillEnable = true;
		if(mCombo)
			mAttackIndex = 0;
		mCombo = false;
		mMovingTimer = 0;
	}

}
