// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCBase.h"

ANPCBase::ANPCBase()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	mTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));

	SetRootComponent(mRoot);
	mMesh->SetupAttachment(mRoot);
	mTrigger->SetupAttachment(mRoot);

	mRoot->SetCollisionProfileName(TEXT("NPC"));
	mTrigger->SetCollisionProfileName(TEXT("NPCTrigger"));

}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();

	mTrigger->OnComponentBeginOverlap.AddDynamic(this, &ANPCBase::NPCBeginOverlap);
	mTrigger->OnComponentEndOverlap.AddDynamic(this, &ANPCBase::NPCEndOverlap);

}

void ANPCBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
