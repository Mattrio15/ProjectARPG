// Fill out your copyright notice in the Description page of Project Settings.


#include "Terra.h"

ATerra::ATerra()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonTerra/Characters/Heroes/Terra/Skins/MountainForge/Meshes/Terra_MountainForge.Terra_MountainForge'"));
	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);

	GetCapsuleComponent()->SetCapsuleHalfHeight(105);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -105));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Character/Terra/ABP_Terra.ABP_Terra_C'"));
	if (AnimClass.Succeeded())
		GetMesh()->SetAnimInstanceClass(AnimClass.Class);

	static ConstructorHelpers::FClassFinder<UGameplayAbility>
		GA_Skill(TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayAbility/Character/GA_Skill_Terra.GA_Skill_Terra_C'"));
	if (GA_Skill.Succeeded())
		mGA_Skill_Terra = GA_Skill.Class;

	mName = TEXT("Terra");
}

void ATerra::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mSkilling)
	{
		UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
		if (IsValid(AIB))
			AIB->SetDirYaw(GetDirYaw());
	}

	if (mUltimating)
	{
		float Height = GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("Height"));
		SetActorLocation(mLocation + FVector(0, 0, Height * 20));
		mArm->TargetArmLength = mArmLen + Height * 10;
		float CameraPitch = GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("CameraPitch"));
		float CameraYaw = GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("CameraYaw"));
		if (CameraPitch < -0.001)
			mArm->SetRelativeRotation(mArm->GetRelativeRotation() + FRotator(DeltaTime * 60, 0, 0));
		else if (CameraPitch > 0.001)
			mArm->SetRelativeRotation(mArm->GetRelativeRotation() + FRotator(DeltaTime * (-150), 0, 0));
		if (CameraYaw > 0)
			mArm->SetRelativeRotation(mArm->GetRelativeRotation() + FRotator(0, DeltaTime * (-255), 0));
	}

}

void ATerra::DisappearAnimation()
{
	UAnimationAsset* AA = LoadObject<UAnimationAsset>(GetWorld(), TEXT("/Script/Engine.AnimSequence'/Game/ParagonTerra/Characters/Heroes/Terra/Animations/Jog_Bwd.Jog_Bwd'"));
	if (IsValid(AA))
		GetMesh()->PlayAnimation(AA, true);
}

void ATerra::ShortAttack()
{
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (!IsValid(AIB))
		return;

	if (!mSkilling)
		AIB->PlayShortAttack();
	else
	{
		mMoveEnable = false;
		mPDEnable = GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.PDEnable")));
		if (!mPDEnable)
			AIB->PlayShortSkill(0);
		else
		{
			UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
			if (IsValid(ASC) && IsValid(mGA_Skill_Terra))
			{
				if (ASC->TryActivateAbilityByClass(mGA_Skill_Terra))
					AIB->PlayShortSkill(1);
			}			
		}
		AIB->SetSkillEnable(false);
	}
}

void ATerra::ShortSkill()
{
	mSkilling = !mSkilling;
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->SetAttackReady(mSkilling);

	float MoveSpeed = mSkilling ? 0.25 : 4;
	GetCharacterMovement()->MaxWalkSpeed *= MoveSpeed;

	GetWorld()->GetTimerManager().SetTimer(mMoveEnableTimer, this, &ATerra::MoveEnableTimer, 1);
	mMoveEnable = false;

	if (!mSkilling)
		mDodgeEnable = true;
}

void ATerra::Ultimate()
{
	mUltimating = true;
	mLocation = GetActorLocation();
	mArmLen = mArm->TargetArmLength;
	mMoveEnable = false;

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayUltimate(0);

	GetWorld()->GetTimerManager().SetTimer(mUltimateTimer, this, &ATerra::UltimateEnd, 4.6);
}

void ATerra::UltimateEffect()
{
	TSubclassOf<UCameraShakeBase> CSB = LoadClass<UCameraShakeBase>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/CameraShake/BS_Terra_Ultimate_CameraShake.BS_Terra_Ultimate_CameraShake_C'"));

	switch (mUltimateEffectCount)
	{
	case 0:
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.25);
		GetWorld()->GetTimerManager().SetTimer(mUltimateDilationTimer, this, &ATerra::UltimateDilationEnd, 0.65);
		break;
	case 1:
		if (IsValid(CSB))
			UGameplayStatics::PlayWorldCameraShake(GetWorld(), CSB, GetActorLocation(), 0, 10000);
		break;
	case 2:
		UNiagaraSystem * NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Vefects/Zap_VFX/VFX/Zap/Particles/NS_Zap_03_Yellow.NS_Zap_03_Yellow'"));
		if (IsValid(NS))
		{
			FHitResult Result;
			for (int32 i = 0; i < 50; ++i)
			{
				FVector Pos1 = GetActorForwardVector() * FMath::RandRange(500, 1000);
				FVector Pos2 = UKismetMathLibrary::GreaterGreater_VectorRotator(Pos1, FRotator(0, FMath::RandRange(-180, 180), 0));
				FVector Pos3 = GetActorLocation() + Pos2;
				GetWorld()->LineTraceSingleByChannel(Result, Pos3 + FVector(0, 0, 10000), Pos3 - FVector(0, 0, 10000), ECollisionChannel::ECC_Camera);
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS, Result.Location);

				TArray<FHitResult> Results;
				bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos3, Pos3, FQuat::Identity, TEXT("PlayerSkill"),
					FCollisionShape::MakeSphere(50));

				if (Collision)
				{
					for (int32 j = 0; j < Results.Num(); ++j)
						GiveDamage(Results[j].GetActor(), 10, false);
				}
			}
		}
	}
	mUltimateEffectCount += 1;
}

void ATerra::DodgeAttack()
{
	Super::DodgeAttack();

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayDodgeAndCounterAttack(0);
}

void ATerra::MoveEnableTimer()
{
	mMoveEnable = true;
}

void ATerra::UltimateEnd()
{
	mUltimating = false;
	mUltimateEnable = true;
	mMoveEnable = true;
	mDodgeEnable = true;

	mUltimateEffectCount = 0;
}

void ATerra::UltimateDilationEnd()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
}

void ATerra::RealAttack(int32 Index)
{
	TArray<FHitResult> Results;
	float Yaw = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance())->GetDirYaw();
	FVector Pos = GetActorLocation() + UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(50, 0, 0), FRotator(0, Yaw, 0));
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(300 + 25 * Index));

	if (Collision)
	{
		UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Terra/NS_Terra_AttackHit.NS_Terra_AttackHit'"));
		if (IsValid(NS))
		{
			ACharacterState* CS = GetPlayerState<ACharacterState>();
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				FVector Pos2 = GetActorLocation() + (Results[i].GetActor()->GetActorLocation() - GetActorLocation()) / 1.5;
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS,
					Pos2, FRotator::ZeroRotator, FVector(10 + Index * 100));

				AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
				if (IsValid(Monster) && IsValid(CS))
					CS->PlayGE_Attack(mName, Monster->GetAbilitySystemComponent());
			}

			USoundBase* SB = LoadObject<USoundBase>(GetWorld(), TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Terra_Attack_Hit.SW_Terra_Attack_Hit'"));
			if (IsValid(SB))
				UGameplayStatics::PlaySound2D(GetWorld(), SB);
		}
	}
}

void ATerra::RealSkill()
{
	if (!mPDEnable)
	{
		TArray<FHitResult> Results;
		float Yaw = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance())->GetDirYaw();
		FVector Pos = GetActorLocation() + UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(200, 0, 0), FRotator(0, Yaw, 0));
		bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
			TEXT("PlayerSkill"), FCollisionShape::MakeSphere(150));

		if (Collision)
		{
			UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonTerra/FX/Particles/Terra/Emotes/FX/P_Terra_Emote_BladeSharpen_PommelHit.P_Terra_Emote_BladeSharpen_PommelHit'"));
			if (IsValid(PS))
			{
				for (int32 i = 0; i < Results.Num(); ++i)
				{
					FVector Pos2 = Results[i].GetActor()->GetActorLocation();
					FVector Pos3 = Pos2 - (Pos2 - GetActorLocation()) / 2;
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, Pos3, FRotator::ZeroRotator, FVector(2));

					GiveDamage(Results[i].GetActor(), 10, false);
				}
			}
		}
	}
	else
	{
		TArray<FHitResult> Results;
		float Yaw = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance())->GetDirYaw();
		FVector Pos = GetActorLocation();
		bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
			TEXT("PlayerSkill"), FCollisionShape::MakeSphere(250));

		if (Collision)
		{
			UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Terra/NS_Terra_SkillHit.NS_Terra_SkillHit'"));
			if (IsValid(NS))
			{
				
				ACharacterState* CS = GetPlayerState<ACharacterState>();

				for (int32 i = 0; i < Results.Num(); ++i)
				{
					FVector Pos2 = Results[i].GetActor()->GetActorLocation();
					FVector Pos3 = Pos2 - (Pos2 - GetActorLocation()) / 2;
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS, Pos3);

					AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
					if (IsValid(Monster) && CS)
					{
						Monster->PlayKnockback(GetActorLocation());
						CS->PlayGE_Skill(mName, Monster->GetAbilitySystemComponent());
					}

				}
			}

			TSubclassOf<UCameraShakeBase> CSB = LoadClass<UCameraShakeBase>(this, TEXT("/Script/Engine.Blueprint'/Game/CameraShake/BS_Terra_Skill_CameraShake.BS_Terra_Skill_CameraShake_C'"));
			if (IsValid(CSB))
				UGameplayStatics::PlayWorldCameraShake(GetWorld(), CSB, GetActorLocation(), 0, 10000);
		}
	}
}

void ATerra::RealDodgeAttack()
{
	TArray<FHitResult> Results;
	float Yaw = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance())->GetDirYaw();
	FVector Pos = GetActorLocation() + UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(50, 0, 0), FRotator(0, Yaw, 0));
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(200));

	if (Collision)
	{
		UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Terra/NS_Terra_AttackHit.NS_Terra_AttackHit'"));
		if (IsValid(NS))
		{
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				FVector Pos2 = GetActorLocation() + (Results[i].GetActor()->GetActorLocation() - GetActorLocation()) / 1.5;
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS,
					Pos2, FRotator::ZeroRotator, FVector(10));

				GiveDamage(Results[i].GetActor(), 10, false);
			}
		}
	}
}

void ATerra::RealUltimate()
{
	TArray<FHitResult> Results;
	bool Collision = GetWorld()->SweepMultiByProfile(Results, GetActorLocation(), GetActorLocation(), FQuat::Identity, TEXT("PlayerSkill"),
		FCollisionShape::MakeSphere(500));

	if (Collision)
	{
		for (int32 i = 0; i < Results.Num(); ++i)
			GiveDamage(Results[i].GetActor(), 10, true);
	}
}
