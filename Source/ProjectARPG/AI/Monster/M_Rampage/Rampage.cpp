// Fill out your copyright notice in the Description page of Project Settings.


#include "Rampage.h"
#include "../../../Character/CharacterBase.h"

ARampage::ARampage()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonRampage/Characters/Heroes/Rampage/Meshes/Rampage.Rampage'"));
	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);

	GetCapsuleComponent()->SetCapsuleHalfHeight(200);
	GetCapsuleComponent()->SetCapsuleRadius(200);

	GetMesh()->SetRelativeLocation(FVector(0, 0, -200));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	GetMesh()->SetRelativeScale3D(FVector(2));

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/AI/Monster/Rampage/ABP_Rampage.ABP_Rampage_C'"));
	if (AnimAsset.Succeeded())
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);

	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		TreeAsset(TEXT("/Script/AIModule.BehaviorTree'/Game/AI/Monster/Rampage/BT_Rampage.BT_Rampage'"));
	if (TreeAsset.Succeeded())
		mTree = TreeAsset.Object;

	GetCharacterMovement()->MaxWalkSpeed = 450;

	mName = TEXT("Rampage");
}

void ARampage::RealAttack()
{
	FHitResult Result;
	FVector Pos = GetActorLocation();
	bool Collision = GetWorld()->SweepSingleByProfile(Result, Pos, Pos, FQuat::Identity, TEXT("MonsterSkill"),
		FCollisionShape::MakeSphere(1000));

	if (Collision)
	{
		TObjectPtr<ACharacterBase> Character = Cast<ACharacterBase>(Result.GetActor());
		if (IsValid(Character))
		{
			TObjectPtr<UAbilitySystemComponent> ASC = Character->GetAbilitySystemComponent();
			if (IsValid(ASC))
			{
				FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
				FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(mGE_MonsterDamage, 1, Context);
				mASC->ApplyGameplayEffectSpecToTarget(*(Spec.Data.Get()), ASC);
			}
		}
	}

}
