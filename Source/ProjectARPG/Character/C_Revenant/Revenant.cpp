// Fill out your copyright notice in the Description page of Project Settings.


#include "Revenant.h"
#include "Revenant_Ultimate.h"

ARevenant::ARevenant()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonRevenant/Characters/Heroes/Revenant/Meshes/Revenant.Revenant'"));
	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);

	GetCapsuleComponent()->SetCapsuleHalfHeight(115);
	GetCapsuleComponent()->SetCapsuleRadius(30);

	GetMesh()->SetRelativeLocation(FVector(0, 0, -115));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Character/Revenant/ABP_Revenant.ABP_Revenant_C'"));
	if (AnimAsset.Succeeded())
		GetMesh()->SetAnimClass(AnimAsset.Class);

	mSkillEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffect"));
	mSkillEffect->SetupAttachment(GetMesh());

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		SkillEffectAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Revenant/NS_Revenant_Skill.NS_Revenant_Skill'"));
	if (SkillEffectAsset.Succeeded())
		mSkillEffect->SetAsset(SkillEffectAsset.Object);
	
	mSkillEffect->bAutoActivate = false;
}

void ARevenant::BeginPlay()
{
	Super::BeginPlay();

	mUltimateActor = GetWorld()->SpawnActor<ARevenant_Ultimate>(ARevenant_Ultimate::StaticClass(),
		FVector(0,0,-10000), mDirScene->GetRelativeRotation());
	if (IsValid(mUltimateActor))
	{
		mUltimateActor->SetActorLocation(mDirScene->GetComponentLocation());
		mUltimateActor->AttachToComponent(mDirScene, FAttachmentTransformRules::KeepWorldTransform);
	}

	mName = TEXT("Revenant");

}

void ARevenant::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	mUltimateActor->Destroy();
}

void ARevenant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mSkilling)
	{
		mSkillDirTimer += DeltaTime;
		if (mSkillDirTimer > 0.2)
		{
			mSkillDir = UKismetMathLibrary::GreaterGreater_VectorRotator(mSkillDir, FRotator(0, mSkillDirPM * 144, 0));
			mSkillDirTimer = 0;
			mSkillVecArray.Add(GetMesh()->GetSocketLocation(TEXT("Muzzle_02")));
			mSkillRotArray.Add(GetMesh()->GetSocketRotation(TEXT("Muzzle_02")));
		}
		AddMovementInput(mSkillDir, 1);

		mArm->TargetArmLength = mArmLen + GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("Camera")) * 5;
	}
}

void ARevenant::DisappearAnimation()
{
	UAnimationAsset* AA = LoadObject<UAnimationAsset>(GetWorld(), TEXT("/Script/Engine.AnimSequence'/Game/ParagonRevenant/Characters/Heroes/Revenant/Animations/Jog_Bwd.Jog_Bwd'"));
	if (IsValid(AA))
		GetMesh()->PlayAnimation(AA, true);
}

void ARevenant::ShortAttack()
{
	if (!mUltimating)
		return;

	mUltimateCount += 1;

	APlayerController* PC = GetController<APlayerController>();

	FHitResult Result;
	FVector MousePos, MouseDir;
	PC->DeprojectMousePositionToWorld(MousePos, MouseDir);
	GetWorld()->LineTraceSingleByProfile(Result, MousePos, MousePos + MouseDir * 10000, TEXT("Player"));
	UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonRevenant/FX/Particles/Revenant/Abilities/Primary/FX/P_Revenant_Primary_HitWorld.P_Revenant_Primary_HitWorld'"));
	if (IsValid(PS))
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, Result.Location, FRotator(90, 0, 0), FVector(5 + mUltimateCount));

	TSubclassOf<UCameraShakeBase> CSB = LoadClass<UCameraShakeBase>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/CameraShake/BS_Revenant_Ultimate_CameraShake.BS_Revenant_Ultimate_CameraShake_C'"));
	if (IsValid(CSB))
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CSB, GetActorLocation(), 0, 10000);

	TArray<FHitResult> Results;
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Result.Location, Result.Location, FQuat::Identity, TEXT("PlayerSkill"),
		FCollisionShape::MakeSphere(300));

	if (Collision)
	{
		for (int32 i = 0; i < Results.Num(); ++i)
			GiveDamage(Results[i].GetActor(), 10, false);
	}

	if (mUltimateCount > 4)
	{
		PC->SetViewTargetWithBlend(this, 1);
		PC->bShowMouseCursor = false;

		GetWorld()->GetTimerManager().SetTimer(mUltimateTimer, this, &ARevenant::UltimateEnd, 1);
		GetMesh()->GetAnimInstance()->StopAllMontages(1);
		mUltimating = false;
	}
}

void ARevenant::LongAttack()
{
	if (mUltimating)
		return;

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayLongAttack();
}

void ARevenant::ShortSkill()
{
	mMoveEnable = false;
	mSkillEnable = false;
	mAttackEnable = false;
	mDodgeEnable = false;
	mSkilling = true;
	mArmLen = mArm->TargetArmLength;

	float RandYaw = FMath::RandRange(-15, 15);
	mSkillDirPM = RandYaw > 0 ? -1 : 1;
	mSkillDir = UKismetMathLibrary::GreaterGreater_VectorRotator(mDirScene->GetForwardVector(), FRotator(0, RandYaw, 0));

	GetCharacterMovement()->MaxAcceleration *= 10;
	GetCharacterMovement()->MaxWalkSpeed *= 10;

	mSkillEffect->Activate(false);
	GetWorld()->GetTimerManager().SetTimer(mSkillTimer, this, &ARevenant::SkillEnd, 1);

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.5);

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayShortSkill(0);
}

void ARevenant::Ultimate()
{
	mCameraRotationEnable = false;
	mUltimating = true;
	mMoveEnable = false;
	mDodgeEnable = false;

	mArmLen = mArm->TargetArmLength;

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
	{
		AIB->SetDirYaw(GetDirYaw());
		AIB->PlayUltimate(0);
	}

	APlayerController* PC = GetController<APlayerController>();
	if (IsValid(mUltimateActor))
		PC->SetViewTargetWithBlend(mUltimateActor, 1);
	PC->bShowMouseCursor = true;
}

void ARevenant::ShortAttackEffect(int32 Index, float Yaw)
{
	UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonRevenant/FX/Particles/Revenant/Abilities/Primary/FX/P_Revenant_Primary_MuzzleFlash.P_Revenant_Primary_MuzzleFlash'"));
	if (IsValid(PS))
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, GetMesh()->GetSocketLocation(TEXT("Muzzle_02")),
			GetMesh()->GetSocketRotation(TEXT("Muzzle_02")), FVector(Index / 3 + 1));

	TSubclassOf<UCameraShakeBase> CSB = LoadClass<UCameraShakeBase>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/CameraShake/BS_Revenant_Attack_CameraShake.BS_Revenant_Attack_CameraShake_C'"));
	if (IsValid(CSB))
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CSB, GetActorLocation(), 0, 10000);
}

void ARevenant::RealAttack(int32 Index)
{
	FHitResult Result;
	FVector Pos = GetActorLocation() + mDirScene->GetForwardVector() * 10000 + FVector(0, 0, 50);
	if (mTarget.IsValid())
		Pos = mTarget->GetActorLocation();
	bool Collision = GetWorld()->LineTraceSingleByProfile(Result, GetActorLocation() + FVector(0, 0, 50), Pos, TEXT("PlayerSkill"));

	if (Collision)
	{
		UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonRevenant/FX/Particles/Revenant/Abilities/Primary/FX/P_Revenant_Primary_HitCharacter.P_Revenant_Primary_HitCharacter'"));
		if (IsValid(PS))
		{
			FVector Pos2 = Result.Location;
			FRotator Rot = (GetActorLocation()-Pos2).Rotation();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, Pos2, Rot, FVector(1 + Index / 2));
		}

		ACharacterState* CB = GetPlayerState<ACharacterState>();
		AMonsterBase* Monster = Cast<AMonsterBase>(Result.GetActor());
		if (IsValid(CB) && IsValid(Monster))
			CB->PlayGE_Attack(mName, Monster->GetAbilitySystemComponent());

		USoundBase* HitSB = LoadObject<USoundBase>(GetWorld(), TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Revenant_Attack_Hit.SW_Revenant_Attack_Hit'"));
		if (IsValid(HitSB))
			UGameplayStatics::PlaySound2D(GetWorld(), HitSB);
	}

}

void ARevenant::RealSkill()
{
	TArray<FHitResult> Results;
	FVector Pos = GetActorLocation();
	Pos -= FVector(0, 0, Pos.Z);
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity, TEXT("PlayerSkill"),
		FCollisionShape::MakeSphere(1000));
	if (Collision)
	{
		UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonRevenant/FX/Particles/Revenant/Abilities/Ultimate/FX/P_Revenant_Ultimate_Hit.P_Revenant_Ultimate_Hit'"));
		if (IsValid(PS))
		{
			ACharacterState* CS = GetPlayerState<ACharacterState>();
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				FVector Pos2 = Results[i].GetActor()->GetActorLocation();
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, Pos2, FRotator::ZeroRotator, FVector(2));
				
				AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
				if (IsValid(CS) && IsValid(Monster))
					CS->PlayGE_Skill(TEXT("Revenant"), Monster->GetAbilitySystemComponent());
			}
		}
	}

}

void ARevenant::SkillEnd()
{
	mDodgeEnable = true;
	mSkilling = false;
	mSkillDirTimer = 0;

	GetCharacterMovement()->MaxAcceleration /= 10;
	GetCharacterMovement()->MaxWalkSpeed /= 10;

	mArm->TargetArmLength = mArmLen;

	mSkillEffect->Deactivate();

	UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonRevenant/FX/Particles/Revenant/Abilities/Obliterate/FX/P_Revenant_Obliterate_Firing.P_Revenant_Obliterate_Firing'"));
	if (IsValid(PS))
	{
		for (int32 i = 0; i < mSkillVecArray.Num(); ++i)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, mSkillVecArray[i], mSkillRotArray[i]);
	}
	mSkillVecArray.Empty();
	mSkillRotArray.Empty();

	TSubclassOf<UCameraShakeBase> CSB = LoadClass<UCameraShakeBase>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/CameraShake/BS_Revenant_Skill_CameraShake.BS_Revenant_Skill_CameraShake_C'"));
	if (IsValid(CSB))
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CSB, GetActorLocation(), 0, 10000);

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
}

void ARevenant::UltimateEnd()
{
	mCameraRotationEnable = true;
	mMoveEnable = true;
	mDodgeEnable = true;

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if(IsValid(AIB))
		AIB->SetAttackEnable(true);

	mUltimateEnable = true;
	mUltimateCount = 0;
}
