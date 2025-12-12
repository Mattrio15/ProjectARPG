// Fill out your copyright notice in the Description page of Project Settings.


#include "Muriel_Attack.h"

AMuriel_Attack::AMuriel_Attack()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	mMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));

	SetRootComponent(mRoot);
	mMovement->SetUpdatedComponent(mRoot);

	mMovement->InitialSpeed = 2000;
	mMovement->ProjectileGravityScale = 0;

	mRoot->SetCollisionProfileName(TEXT("PlayerSkill"));

	SetLifeSpan(2);
}

void AMuriel_Attack::BeginPlay()
{
	Super::BeginPlay();
	
	UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/Character/Muriel/P_Muriel_Primary_Projectile.P_Muriel_Primary_Projectile'"));
	if (IsValid(PS))
	{
		UGameplayStatics::SpawnEmitterAttached(PS, mRoot, TEXT(""), mRoot->GetComponentLocation(), mRoot->GetRelativeRotation(),
			EAttachLocation::KeepWorldPosition);
	}

	mMovement->OnProjectileStop.AddDynamic(this, &AMuriel_Attack::AttackHit);
}

void AMuriel_Attack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMuriel_Attack::AttackHit(const FHitResult& ImpactResult)
{
	TArray<FHitResult> Results;
	FVector Pos = GetActorLocation();
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(100));

	if (Collision && IsValid(mParent))
	{
		ACharacterState* CS = mParent->GetPlayerState<ACharacterState>();
		if (IsValid(CS))
		{
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
				if (IsValid(Monster))
					CS->PlayGE_Attack(TEXT("Muriel"), Monster->GetAbilitySystemComponent());
			}
		}
	}

	UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMuriel/FX/Particles/Abilities/Primary/FX/P_Muriel_Primary_HitCharacter.P_Muriel_Primary_HitCharacter'"));
	if (IsValid(PS))
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, Pos, FRotator::ZeroRotator, FVector(3));

	USoundBase* SB = LoadObject<USoundBase>(GetWorld(), TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Muriel_Attack_Hit.SW_Muriel_Attack_Hit'"));
	if (IsValid(SB))
		UGameplayStatics::PlaySound2D(GetWorld(), SB);

	Destroy();
}

