// Fill out your copyright notice in the Description page of Project Settings.


#include "TwinBlast.h"
#include "TwinBlast_Ultimate_Actor.h"

ATwinBlast::ATwinBlast()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonTwinblast/Characters/Heroes/TwinBlast/Meshes/TwinBlast.TwinBlast'"));
	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);

	GetCapsuleComponent()->SetCapsuleHalfHeight(95);
	GetCapsuleComponent()->SetCapsuleRadius(30);

	GetMesh()->SetRelativeLocation(FVector(0, 0, -95));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Character/TwinBlast/ABP_TwinBlast.ABP_TwinBlast_C'"));
	if (AnimAsset.Succeeded())
		GetMesh()->SetAnimClass(AnimAsset.Class);

	mSkillEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffect"));
	mSkillEffect->SetupAttachment(GetMesh(), TEXT("SkillSocket"));
	mSkillEffect->SetRelativeRotation(FRotator(0, 0, 90));
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		EffectAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/TwinBlast/NS_TwinBlast_Skill.NS_TwinBlast_Skill'"));
	if (EffectAsset.Succeeded())
		mSkillEffect->SetAsset(EffectAsset.Object);

	mSkillEffect->bAutoActivate = false;

	static ConstructorHelpers::FClassFinder<UGameplayAbility>
		GA_Skill(TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayAbility/Character/GA_Skill_TwinBlast.GA_Skill_TwinBlast_C'"));
	if (GA_Skill.Succeeded())
		mGA_Skill_TwinBlast = GA_Skill.Class;

	mName = TEXT("TwinBlast");
}

void ATwinBlast::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!mSkillEnable && !mSkilling)
	{
		if (mDodge)
			return;
		UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
		if (IsValid(AIB))
			AIB->SetDirYaw(mDirScene->GetRelativeRotation().Yaw);
	}
	if (mUltimating)
	{
		float Len = GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("Camera"));
		mArm->TargetArmLength = mArmLength + Len * 7.5;
		mArm->SetRelativeLocation(mCameraPos + FVector(0, 0, Len * 3.5));
	}
}

void ATwinBlast::DisappearAnimation()
{
	UAnimationAsset* AA = LoadObject<UAnimationAsset>(GetWorld(), TEXT("/Script/Engine.AnimSequence'/Game/ParagonTwinblast/Characters/Heroes/TwinBlast/Animations/Jog_Bwd.Jog_Bwd'"));
	if (IsValid(AA))
		GetMesh()->PlayAnimation(AA, true);
}

void ATwinBlast::LongAttack()
{
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
	{
		if (mSkillEnable)
			AIB->PlayLongAttack();
		else
		{
			if (mSkilling)
				return;
			mSkilling = true;

			UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
			if (IsValid(ASC) && IsValid(mGA_Skill_TwinBlast))
			{
				if (!ASC->TryActivateAbilityByClass(mGA_Skill_TwinBlast))
					return;
			}

			AIB->PlayLongSkill(2);
			GetWorld()->GetTimerManager().SetTimer(mSkillTimer, this, &ATwinBlast::SkillFireEnd, 0.7);

			TSubclassOf<UCameraShakeBase> CSB = LoadClass<UCameraShakeBase>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/CameraShake/BC_TwinBlast_Skill_CameraShake.BC_TwinBlast_Skill_CameraShake_C'"));
			if (IsValid(CSB))
				UGameplayStatics::PlayWorldCameraShake(GetWorld(), CSB, RootComponent->GetComponentLocation(), 0, 1000);
		}
	}
}

void ATwinBlast::LongSkillReady()
{
	mSkillEnable = false;
	mMoveEnable = false;

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayLongSkill(0);

}

void ATwinBlast::LongSkillCompleted()
{
	mSkillEffect->Deactivate();
	mDodgeEnable = true;

	if (mSkillEnable)
		return;

	mSkillEnable = true;

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayLongSkill(1);

}

void ATwinBlast::Ultimate()
{
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayUltimate(0);

	mMoveEnable = false;

	mUltimating = true;
	mArmLength = mArm->TargetArmLength;
	mCameraPos = mArm->GetRelativeLocation();

	GetWorld()->GetTimerManager().SetTimer(mUltimateTimer, this, &ATwinBlast::UltimateEnd, 4);
}

void ATwinBlast::SkillFireEnd()
{
	mSkilling = false;
}

void ATwinBlast::UltimateEnd()
{
	mMoveEnable = true;
	mDodgeEnable = true;
	mUltimateEnable = true;
	mUltimating = false;
	GetMesh()->GetAnimInstance()->StopAllMontages(0.5);
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
	{
		AIB->SetAttackEnable(true);
		AIB->SetSkillEnable(true);
	}
}

void ATwinBlast::ShortAttackEffect(int32 Index, float Yaw)
{
	TSubclassOf<UCameraShakeBase> CSB = LoadClass<UCameraShakeBase>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/CameraShake/BC_TwinBlast_Attack_CameraShake.BC_TwinBlast_Attack_CameraShake_C'"));

	if (IsValid(CSB))
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CSB, RootComponent->GetComponentLocation(), 0, 1000);
}

void ATwinBlast::LongSkillEffect(int32 Index, float Yaw)
{
	if (mSkillEffect->IsActive())
		mSkillEffect->Deactivate();
	else
		mSkillEffect->Activate(false);
}

void ATwinBlast::UltimateEffect()
{
	ATwinBlast_Ultimate_Actor* TUA = GetWorld()->SpawnActor<ATwinBlast_Ultimate_Actor>(ATwinBlast_Ultimate_Actor::StaticClass(), GetMesh()->GetSocketLocation(TEXT("FX_Grenade")), FRotator(90, 0, 0));
	if (IsValid(TUA))
		TUA->SetParent(this);

}

void ATwinBlast::RealAttack(int32 Index)
{
	FHitResult Result;
	FVector Pos = GetActorLocation() + mDirScene->GetForwardVector() * 10000 + FVector(0, 0, 50);
	if (IsValid(mTarget))
		Pos = mTarget->GetActorLocation() + FVector(0, 0, 50);
	bool Collision = GetWorld()->LineTraceSingleByProfile(Result, GetActorLocation() + FVector(0, 0, 50), Pos, TEXT("PlayerSkill"));

	if (Collision)
	{
		UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonTwinblast/FX/Particles/Abilities/Nitro/FX/P_TwinBlast_Nitro_HitCharacter.P_TwinBlast_Nitro_HitCharacter'"));
		if (IsValid(PS))
		{
			FVector Pos2 = Result.Location;
			FRotator Rot = (GetActorLocation() - Pos2).Rotation();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, Pos2, Rot);

			AMonsterBase* Monster = Cast<AMonsterBase>(Result.GetActor());
			ACharacterState* CB = GetPlayerState<ACharacterState>();
			if (IsValid(Monster) && IsValid(CB))
				CB->PlayGE_Attack(mName, Monster->GetAbilitySystemComponent());
		}
	}
}

void ATwinBlast::RealSkill()
{
	FHitResult Result;
	FVector Pos = GetActorLocation() + mDirScene->GetForwardVector() * 10000 + FVector(0, 0, 50);
	if (IsValid(mTarget))
		Pos = mTarget->GetActorLocation() + FVector(0, 0, 50);
	bool Collision = GetWorld()->LineTraceSingleByProfile(Result, GetActorLocation() + FVector(0, 0, 50), Pos, TEXT("PlayerSkill"));

	if (Collision)
	{
		UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/MixedVFX/Particles/Explosions/NS_ExplosionMidAirSmall.NS_ExplosionMidAirSmall'"));
		if (IsValid(NS))
		{
			FVector Pos2 = Result.Location;
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS, Pos2);
			
			TArray<FHitResult> Results;
			bool Collision2 = GetWorld()->SweepMultiByProfile(Results, Pos2, Pos2, FQuat::Identity, TEXT("PlayerSkill"),
				FCollisionShape::MakeSphere(200));

			if (Collision2)
			{
				ACharacterState* CS = GetPlayerState<ACharacterState>();
				if (IsValid(CS))
				{
					for (int32 i = 0; i < Results.Num(); ++i)
					{
						AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
						if (IsValid(Monster))
							CS->PlayGE_Skill(mName, Monster->GetAbilitySystemComponent());
					}
				}
			}
		}
	}

}
