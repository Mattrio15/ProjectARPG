// Fill out your copyright notice in the Description page of Project Settings.


#include "TwinBlast_Grenade.h"

ATwinBlast_Grenade::ATwinBlast_Grenade()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	mMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	mEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Effect"));

	SetRootComponent(mRoot);
	mMesh->SetupAttachment(mRoot);
	mMovement->SetUpdatedComponent(mRoot);
	mEffect->SetupAttachment(mRoot);

	mMesh->SetRelativeRotation(FRotator(0, -90, 0));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Character/TwinBlast/SM_Twinblast_Grenade.SM_Twinblast_Grenade'"));
	if (MeshAsset.Succeeded())
		mMesh->SetStaticMesh(MeshAsset.Object);

	mMovement->InitialSpeed = 1000;
	mMovement->ProjectileGravityScale = 1;
	mMovement->bRotationFollowsVelocity = true;

	mMesh->SetRelativeScale3D(FVector(5));

	mRoot->SetCollisionProfileName(TEXT("Player"));
	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATwinBlast_Grenade::BeginPlay()
{
	Super::BeginPlay();
	
	mMovement->OnProjectileStop.AddDynamic(this, &ATwinBlast_Grenade::GrenadeBomb);
}

void ATwinBlast_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATwinBlast_Grenade::GrenadeBomb(const FHitResult& ImpactResult)
{
	UNiagaraSystem* NS = nullptr;
	TArray<FHitResult> Results;
	bool Collision = false;
	switch (mIndex)
	{
	case 0:
		NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/MixedVFX/Particles/Explosions/NS_ExplosionGroundSmall.NS_ExplosionGroundSmall'"));

		Collision = GetWorld()->SweepMultiByProfile(Results, GetActorLocation(), GetActorLocation(), FQuat::Identity,
			TEXT("PlayerSkill"), FCollisionShape::MakeSphere(100));
		break;
	case 1:
		NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/MixedVFX/Particles/Explosions/NS_ExplosionGroundMedium.NS_ExplosionGroundMedium'"));

		Collision = GetWorld()->SweepMultiByProfile(Results, GetActorLocation(), GetActorLocation(), FQuat::Identity,
			TEXT("PlayerSkill"), FCollisionShape::MakeSphere(200));
		break;
	case 2:
		NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/MixedVFX/Particles/Explosions/NS_ExplosionGroundBig.NS_ExplosionGroundBig'"));

		Collision = GetWorld()->SweepMultiByProfile(Results, GetActorLocation(), GetActorLocation(), FQuat::Identity,
			TEXT("PlayerSkill"), FCollisionShape::MakeSphere(300));
		break;
	}
	if (Collision)
	{
		for (int32 i = 0; i < Results.Num(); ++i)
			mParent->GiveDamage(Results[i].GetActor(), 10, false);
	}

	if (IsValid(NS))
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS, ImpactResult.Location);

	TSubclassOf<UCameraShakeBase> CSB = LoadClass<UCameraShakeBase>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/CameraShake/BC_TwinBlast_Ultimate_CameraShake.BC_TwinBlast_Ultimate_CameraShake_C'"));
	if (IsValid(CSB))
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CSB, mRoot->GetComponentLocation(), 0, 10000);

	Destroy();
}

