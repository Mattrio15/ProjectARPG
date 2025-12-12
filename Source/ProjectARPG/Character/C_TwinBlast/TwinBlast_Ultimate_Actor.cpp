// Fill out your copyright notice in the Description page of Project Settings.


#include "TwinBlast_Ultimate_Actor.h"
#include "TwinBlast_Grenade.h"

ATwinBlast_Ultimate_Actor::ATwinBlast_Ultimate_Actor()
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

	mRoot->SetRelativeScale3D(FVector(0.25));
	mMesh->SetRelativeRotation(FRotator(180, 0, 0));
	mEffect->SetRelativeRotation(FRotator(180, 0, 0));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Character/TwinBlast/SM_Twinblast_Ultimate.SM_Twinblast_Ultimate'"));
	if (MeshAsset.Succeeded())
		mMesh->SetStaticMesh(MeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		EffectAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/TwinBlast/NS_Twinblast_Ultimate.NS_Twinblast_Ultimate'"));
	if (EffectAsset.Succeeded())
		mEffect->SetAsset(EffectAsset.Object);

	mMovement->InitialSpeed = 1000;
	mMovement->ProjectileGravityScale = 1;
	
	mRoot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATwinBlast_Ultimate_Actor::BeginPlay()
{
	Super::BeginPlay();
	
	mHeight = mRoot->GetComponentLocation().Z;
}

void ATwinBlast_Ultimate_Actor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/MixedVFX/Particles/Explosions/NS_ExplosionMidAirSmall.NS_ExplosionMidAirSmall'"));
	if (IsValid(NS))
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS, mRoot->GetComponentLocation());

	for (int32 i = 0; i < 15; ++i)
	{
		ATwinBlast_Grenade* AG1 = GetWorld()->SpawnActor<ATwinBlast_Grenade>(ATwinBlast_Grenade::StaticClass(),
			RootComponent->GetComponentLocation(), FRotator(-50, i * 24, 0));
		if (IsValid(AG1))
		{
			AG1->SetIndex(0);
			AG1->SetParent(mParent);
		}
		ATwinBlast_Grenade* AG2 = GetWorld()->SpawnActor<ATwinBlast_Grenade>(ATwinBlast_Grenade::StaticClass(),
			RootComponent->GetComponentLocation(), FRotator(-15, i * 24, 0));
		if (IsValid(AG2))
		{
			AG2->SetIndex(1);
			AG2->SetParent(mParent);
		}
		ATwinBlast_Grenade* AG3 = GetWorld()->SpawnActor<ATwinBlast_Grenade>(ATwinBlast_Grenade::StaticClass(),
			RootComponent->GetComponentLocation(), FRotator(25, i * 24, 0));
		if (IsValid(AG3))
		{
			AG3->SetIndex(2);
			AG3->SetParent(mParent);
		}
	}
}

void ATwinBlast_Ultimate_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	mHeight = mRoot->GetComponentLocation().Z;
	if (mHeight > mMaxHeight)
		mMaxHeight = mHeight;
	else
		Destroy();
}

