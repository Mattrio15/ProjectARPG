// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCMiniGame.h"
#include "../NPCAnimInstance.h"
#include "../../../Character/CharacterBase.h"

ANPCMiniGame::ANPCMiniGame()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonYin/Characters/Heroes/Yin/Meshes/Yin.Yin'"));
	if (MeshAsset.Succeeded())
		mMesh->SetSkeletalMesh(MeshAsset.Object);

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/AI/NPC/MiniGame/ABP_MiniGame.ABP_MiniGame_C'"));
	if (AnimAsset.Succeeded())
		mMesh->SetAnimInstanceClass(AnimAsset.Class);

	mRoot->SetCapsuleHalfHeight(85);
	mRoot->SetCapsuleRadius(20);

	mMesh->SetRelativeLocation(FVector(0,0,-85));
	mMesh->SetRelativeRotation(FRotator(0, -90, 0));

	mTrigger->SetSphereRadius(250);

}

void ANPCMiniGame::NPCBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::NPCBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
	if (!IsValid(Character))
		return;

	Character->SetNPCTalking(true);
}

void ANPCMiniGame::NPCEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::NPCEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
	if (!IsValid(Character))
		return;

	Character->SetNPCTalking(false);
}
