// Fill out your copyright notice in the Description page of Project Settings.


#include "Muriel_Ultimate.h"

AMuriel_Ultimate::AMuriel_Ultimate()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	mMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));

	SetRootComponent(mRoot);
	mMovement->SetUpdatedComponent(mRoot);

	mMovement->InitialSpeed = 2000;
	mMovement->bIsHomingProjectile = true;
	mMovement->ProjectileGravityScale = 0;
	mMovement->bRotationFollowsVelocity = true;

	mRoot->SetCollisionProfileName(TEXT("Player"));

}

void AMuriel_Ultimate::BeginPlay()
{
	Super::BeginPlay();

	UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMuriel/FX/Particles/Abilities/LifeLock/FX/P_LifeLock_Projectile.P_LifeLock_Projectile'"));
	if (IsValid(PS))
		UGameplayStatics::SpawnEmitterAttached(PS, mRoot, TEXT(""), GetActorLocation(), GetActorRotation(), FVector(0.5), EAttachLocation::KeepWorldPosition);
	
	mMovement->OnProjectileStop.AddDynamic(this, &AMuriel_Ultimate::UltimateBomb);
}

void AMuriel_Ultimate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMuriel_Ultimate::SetHomingTarget(USceneComponent* Target)
{
	mMovement->HomingTargetComponent = Target;
}

void AMuriel_Ultimate::SetSpeed(float Speed)
{
	mMovement->MaxSpeed = Speed;
	mMovement->HomingAccelerationMagnitude = Speed/3;
}

void AMuriel_Ultimate::UltimateBomb(const FHitResult& ImpactResult)
{
	UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Muriel/NS_Muriel_Ultimate.NS_Muriel_Ultimate'"));
	if (IsValid(NS))
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS, GetActorLocation(), FRotator::ZeroRotator, FVector(2));

	TSubclassOf<UCameraShakeBase> CSB = LoadClass<UCameraShakeBase>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/CameraShake/BS_Muriel_Ultimate_CameraShake.BS_Muriel_Ultimate_CameraShake_C'"));
	if (IsValid(CSB))
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CSB, GetActorLocation(), 0, 10000);

	UltimateHit();
}

void AMuriel_Ultimate::UltimateHit()
{
	mUltimateCount += 1;
	GetWorld()->GetTimerManager().SetTimer(mUltimateHitTimer, this, &AMuriel_Ultimate::UltimateHit, 0.5, true);

	TArray<FHitResult> Results;
	bool Collision = GetWorld()->SweepMultiByProfile(Results, GetActorLocation(), GetActorLocation(), FQuat::Identity, TEXT("PlayerSkill"),
		FCollisionShape::MakeSphere(300));

	if (Collision)
	{
		for (int32 i = 0; i < Results.Num(); ++i)
		{
			if (IsValid(mParent))
				mParent->GiveDamage(Results[i].GetActor(), 10, false);
		}
	}

	if (mUltimateCount > 5)
		Destroy();
}

