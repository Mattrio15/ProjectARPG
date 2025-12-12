// Fill out your copyright notice in the Description page of Project Settings.


#include "Kallari.h"

AKallari::AKallari()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonKallari/Characters/Heroes/Kallari/Meshes/Kallari.Kallari'"));
	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);

	GetCapsuleComponent()->SetCapsuleHalfHeight(90);
	GetCapsuleComponent()->SetCapsuleRadius(25);

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Character/Kallari/ABP_Kallari.ABP_Kallari_C'"));
	if (AnimAsset.Succeeded())
		GetMesh()->SetAnimClass(AnimAsset.Class);

	mName = TEXT("Kallari");

}

void AKallari::DisappearAnimation()
{
	UAnimationAsset* AA = LoadObject<UAnimationAsset>(GetWorld(), TEXT("/Script/Engine.AnimSequence'/Game/ParagonKallari/Characters/Heroes/Kallari/Animations/Jog_Bwd.Jog_Bwd'"));
	if (IsValid(AA))
		GetMesh()->PlayAnimation(AA, true);
}

void AKallari::ShortAttack()
{
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayShortAttack();
}

void AKallari::DodgeAttack()
{
	Super::DodgeAttack();
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayDodgeAndCounterAttack(0);
}

void AKallari::RealAttack(int32 Index)
{
	TArray<FHitResult> Results;
	float DirYaw = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance())->GetDirYaw();
	FRotator Rot = FRotator(0, DirYaw, 0);
	FVector Pos = GetActorLocation() + UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(100, 0, 0), Rot);
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity, 
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(150));

	if (Collision)
	{
		UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonKallari/FX/Particles/Kallari/Abilities/Primary/FX/P_Kallari_Melee_SucessfulImpact.P_Kallari_Melee_SucessfulImpact'"));
		if (IsValid(PS))
		{
			ACharacterState* CS = GetPlayerState<ACharacterState>();
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				FVector Pos2 = GetActorLocation() + (Results[i].GetActor()->GetActorLocation() - GetActorLocation()) / 2;
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS,
					Pos2, FRotator::ZeroRotator, FVector(1 + Index));

				AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
				if (IsValid(CS) && IsValid(Monster))
					CS->PlayGE_Attack(TEXT("Kallari"), Monster->GetAbilitySystemComponent());
			}
		}
	}

}

void AKallari::RealDodgeAttack()
{
	TArray<FHitResult> Results;
	float Yaw = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance())->GetDirYaw();
	FVector Pos = GetActorLocation() + UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(50, 0, 0), FRotator(0, Yaw, 0));
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(200));

	if (Collision)
	{
		UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonKallari/FX/Particles/Kallari/Abilities/Primary/FX/P_Kallari_Melee_SucessfulImpact.P_Kallari_Melee_SucessfulImpact'"));
		for (int32 i = 0; i < Results.Num(); ++i)
		{
			FVector Pos2 = GetActorLocation() + (Results[i].GetActor()->GetActorLocation() - GetActorLocation()) / 2;
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS,
				Pos2, FRotator::ZeroRotator, FVector(2));

			GiveDamage(Results[i].GetActor(), 10, false);
		}
	}
}
