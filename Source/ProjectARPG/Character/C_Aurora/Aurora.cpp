// Fill out your copyright notice in the Description page of Project Settings.


#include "Aurora.h"

AAurora::AAurora()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonAurora/Characters/Heroes/Aurora/Meshes/Aurora.Aurora'"));
	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);

	GetCapsuleComponent()->SetCapsuleHalfHeight(90);
	GetCapsuleComponent()->SetCapsuleRadius(25);

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Character/Aurora/ABP_Aurora.ABP_Aurora_C'"));
	if (AnimAsset.Succeeded())
		GetMesh()->SetAnimClass(AnimAsset.Class);

	mUltimateEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("UltimateEffect"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		EffectAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Aurora/NS_Aurora_Ultimate.NS_Aurora_Ultimate'"));
	if (EffectAsset.Succeeded())
		mUltimateEffect->SetAsset(EffectAsset.Object);

	mUltimateEffect->SetupAttachment(GetMesh(), TEXT("Sword_Tip"));
	mUltimateEffect->SetRelativeScale3D(FVector(5));
	mUltimateEffect->bAutoActivate = false;

	mName = TEXT("Aurora");

	static ConstructorHelpers::FClassFinder<UGameplayAbility>
		GA_Skill_Aurora(TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayAbility/Character/GA_Skill_Aurora.GA_Skill_Aurora_C'"));
	if (GA_Skill_Aurora.Succeeded())
		mGA_Skill_Aurora = GA_Skill_Aurora.Class;

	static ConstructorHelpers::FObjectFinder<USoundBase>
		SB_Attack(TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Aurora_Attack.SW_Aurora_Attack'"));
	if (SB_Attack.Succeeded())
		mSB_Attack = SB_Attack.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase>
		SB_Skill(TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Aurora_Skill.SW_Aurora_Skill'"));
	if (SB_Skill.Succeeded())
		mSB_Skill = SB_Skill.Object;

}

void AAurora::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mUltimating)
	{
		float Height = GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("Height"));
		SetActorLocation(mLocation + FVector(0, 0, Height * 5));
		mArm->TargetArmLength = mArmLength + Height * 7.5;

		mUTimer += DeltaTime;
		if (mUTimer > 0.08)
		{
			if (mUCount < 8)
			{
				mUTimer = 0;
				FRotator Rot = FRotator(0, mUArray[mUCount] * 360 / 8 + 22.5, 0);
				FVector RandVec = UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(500, 0, 0), Rot) + GetActorLocation();
				FHitResult Result;
				GetWorld()->LineTraceSingleByChannel(Result, RandVec + FVector(0, 0, 10000), RandVec - FVector(0, 0, 10000), ECollisionChannel::ECC_Camera);
				UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonAurora/FX/Particles/Abilities/Leap/FX/P_Aurora_Decoy_Frost.P_Aurora_Decoy_Frost'"));
				if (IsValid(PS))
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, Result.Location, FRotator::ZeroRotator, FVector(3));
				mUCount += 1;

				TArray<FHitResult> Results;
				bool Collision = GetWorld()->SweepMultiByProfile(Results, Result.Location, Result.Location, FQuat::Identity, TEXT("PlayerSkill"),
					FCollisionShape::MakeSphere(300));

				for(int32 i=0; i<Results.Num();++i)
					GiveDamage(Results[i].GetActor(), 10, false);
			}
		}
	}

}

void AAurora::DisappearAnimation()
{
	UAnimationAsset* AA = LoadObject<UAnimationAsset>(GetWorld(), TEXT("/Script/Engine.AnimSequence'/Game/ParagonAurora/Characters/Heroes/Aurora/Animations/Jog_Bwd.Jog_Bwd'"));
	if (IsValid(AA))
		GetMesh()->PlayAnimation(AA, true);
}

void AAurora::ShortAttack()
{
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (!IsValid(AIB))
		return;

	AIB->PlayShortAttack();
}

void AAurora::ShortSkill()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if(IsValid(ASC) && mGA_Skill_Aurora)
		ASC->TryActivateAbilityByClass(mGA_Skill_Aurora);
}

void AAurora::Ultimate()
{
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
	{
		AIB->PlayUltimate(0);
		AIB->SetDirYaw(mDirScene->GetRelativeRotation().Yaw);
	}

	mLocation = GetActorLocation();
	mArmLength = mArm->TargetArmLength;
	mUltimating = true;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.25);
	GetWorld()->GetTimerManager().SetTimer(mUltimateTimer, this, &AAurora::FinishUltimateDilation, 1);

	mUltimateEffect->Activate(false);
	mUltimateDilation = true;

	mUCount = 0;
	mUArray.Empty();

	while (true)
	{
		int32 Index = FMath::RandRange(0, 7);
		if (!mUArray.Contains(Index))
			mUArray.Add(Index);
		if (mUArray.Num() == 8)
			break;
	}

}

void AAurora::DodgeAttack()
{
	Super::DodgeAttack();
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayDodgeAndCounterAttack(0);
}

void AAurora::ShortAttackEffect(int32 Index, float Yaw)
{
	UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Aurora/NS_Aurora_Attack.NS_Aurora_Attack'"));
	if (IsValid(NS))
	{
		FVector Pos = GetActorLocation() + FVector(0, 0, 40);
		FRotator Rot = FRotator(0, Yaw, 0);
		switch (Index)
		{
		case 0:
			Rot += FRotator(0, 180, 20);
			break;
		case 1:
			Rot += FRotator(0, 180, 135);
			break;
		case 2:
			Rot += FRotator(0, 180, 45);
			break;
		case 3:
			Rot += FRotator(0, 190, 240);
			break;
		}
		UNiagaraFunctionLibrary::SpawnSystemAttached(NS, RootComponent, FName(TEXT("")), Pos, Rot, FVector(1.5), 
			EAttachLocation::KeepWorldPosition, true, ENCPoolMethod::AutoRelease);

		if (IsValid(mSB_Attack))
			UGameplayStatics::PlaySound2D(GetWorld(), mSB_Attack);
	}
}

void AAurora::ShortSkillEffect(int32 Index, float Yaw)
{
	UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonAurora/FX/Particles/Abilities/Freeze/FX/P_Aurora_Freeze_Whrilwind.P_Aurora_Freeze_Whrilwind'"));
	if (IsValid(PS))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, GetActorLocation());
		UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Aurora/NS_Aurora_Skill.NS_Aurora_Skill'"));
		if (IsValid(NS))
		{
			FVector Pos = GetActorLocation() + FVector(0, 0, 20);
			FRotator Rot = FRotator(0, Yaw + 200, 10);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS, Pos, Rot, FVector(2.5));

			if (IsValid(mSB_Skill))
				UGameplayStatics::PlaySound2D(GetWorld(), mSB_Skill);
		}
	}
}

void AAurora::RealAttack(int32 Index)
{
	TArray<FHitResult> Results;
	float Yaw = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance())->GetDirYaw();
	FVector Pos = GetActorLocation() + UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(50, 0, 0), FRotator(0, Yaw, 0));
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(200));

	if (Collision)
	{
		UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Aurora/NS_Aurora_AttackHit.NS_Aurora_AttackHit'"));
		if (IsValid(NS))
		{
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				FVector Pos2 = GetActorLocation() + (Results[i].GetActor()->GetActorLocation() - GetActorLocation()) / 2;
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS,
					Pos2, FRotator::ZeroRotator, FVector(10 + Index*100));

				ACharacterState* CS = GetPlayerState<ACharacterState>();
				AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
				if (IsValid(CS) && IsValid(Monster))
					CS->PlayGE_Attack(mName, Monster->GetAbilitySystemComponent());

				USoundBase* SB = LoadObject<USoundBase>(GetWorld(), TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Aurora_Attack_Hit.SW_Aurora_Attack_Hit'"));
				if (IsValid(SB))
					UGameplayStatics::PlaySound2D(GetWorld(), SB);
			}
		}
	}
}

void AAurora::RealSkill()
{
	TArray<FHitResult> Results;
	FVector Pos = GetActorLocation();
	Pos -= FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(750));

	if (Collision)
	{
		UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonAurora/FX/Particles/Abilities/Primary/FX/P_Aurora_Melee_SucessfulImpact.P_Aurora_Melee_SucessfulImpact'"));
		if (IsValid(PS))
		{
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, 
					Results[i].GetActor()->GetActorLocation(), FRotator::ZeroRotator, FVector(5));

				ACharacterState* CS = GetPlayerState<ACharacterState>();
				AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
				if (IsValid(CS)&&IsValid(Monster))
					CS->PlayGE_Skill(mName, Monster->GetAbilitySystemComponent());

				USoundBase* SB = LoadObject<USoundBase>(GetWorld(), TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Aurora_Skill_Hit.SW_Aurora_Skill_Hit'"));
				if (IsValid(SB))
					UGameplayStatics::PlaySound2D(GetWorld(), SB);
			}
		}
	}
}

void AAurora::RealDodgeAttack()
{
	TArray<FHitResult> Results;
	float Yaw = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance())->GetDirYaw();
	FVector Pos = GetActorLocation() + UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(50, 0, 0), FRotator(0, Yaw, 0));
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(200));

	if (Collision)
	{
		UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Aurora/NS_Aurora_AttackHit.NS_Aurora_AttackHit'"));
		if (IsValid(NS))
		{
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				FVector Pos2 = GetActorLocation() + (Results[i].GetActor()->GetActorLocation() - GetActorLocation()) / 2;
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS,
					Pos2, FRotator::ZeroRotator, FVector(10));

				GiveDamage(Results[i].GetActor(), 10, false);
			}
		}
	}
}

void AAurora::RealUltimate()
{
	FHitResult Result;
	GetWorld()->LineTraceSingleByChannel(Result, GetActorLocation(), GetActorLocation() - FVector(0, 0, 10000), ECollisionChannel::ECC_Camera);

	TArray<FHitResult> Results;
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Result.Location, Result.Location, FQuat::Identity, TEXT("PlayerSkill"),
		FCollisionShape::MakeSphere(1000));

	for(int32 i=0; i<Results.Num();++i)
		GiveDamage(Results[i].GetActor(), 10, true);
}

void AAurora::FinishUltimateDilation()
{
	mUltimating = false;
	mArm->TargetArmLength = mArmLength;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);

	mUltimateEffect->Deactivate();

	mUltimateDilation = false;

	mUltimateEnable = true;
	mDodgeEnable = true;
}
