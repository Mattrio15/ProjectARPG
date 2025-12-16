// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterController.h"
#include "MonsterBase.h"
#include "../../Character/CharacterBase.h"

AMonsterController::AMonsterController()
{
	mPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	SetPerceptionComponent(*mPerception);

	mAISenseSight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISenseSight"));

	mAISenseSight->SightRadius = 3000;
	mAISenseSight->LoseSightRadius = 3500;
	mAISenseSight->PeripheralVisionAngleDegrees = 135;

	PerceptionComponent->ConfigureSense(*mAISenseSight);
	PerceptionComponent->SetDominantSense(mAISenseSight->GetSenseImplementation());

}

void AMonsterController::BeginPlay()
{
	Super::BeginPlay();

	mPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterController::TargetUpdate);

}

void AMonsterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(Blackboard->GetValueAsObject(TEXT("Target"))))
	{
		for (int32 i = 0; i < mTargets.Num(); ++i)
		{
			if (!IsValid(mTargets[i]))
			{
				mTargets.RemoveAtSwap(i);
				i = -1;
				continue;
			}
			Blackboard->SetValueAsObject(TEXT("Target"), mTargets[i]);
			break;
		}
	}
}

void AMonsterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AMonsterBase* Monster = Cast<AMonsterBase>(InPawn);
	if (IsValid(Monster))
		RunBehaviorTree(Monster->GetTree());
}

void AMonsterController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AMonsterController::SetPD()
{
	// ACharacterBase* CB = Cast<ACharacterBase>(Blackboard->GetValueAsObject(TEXT("Target")));
	// if (IsValid(CB))
	// 	CB->SetPDEnable(GetPawn());

	AMonsterBase* Monster = GetPawn<AMonsterBase>();
	ACharacterBase* CB = Cast<ACharacterBase>(Blackboard->GetValueAsObject(TEXT("Target")));
	if (!IsValid(Monster) || !IsValid(CB))
		return;
	UAbilitySystemComponent* MonsterASC = Monster->GetAbilitySystemComponent();
	UAbilitySystemComponent* CharacterASC = CB->GetAbilitySystemComponent();
	if (!IsValid(MonsterASC) || !IsValid(CharacterASC))
		return;
	TSubclassOf<UGameplayEffect> GE_PDEnable = LoadClass<UGameplayEffect>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayEffect/Monster/GE_PDEnable.GE_PDEnable_C'"));
	if (!IsValid(GE_PDEnable))
		return;
	MonsterASC->ApplyGameplayEffectToTarget(GE_PDEnable->GetDefaultObject<UGameplayEffect>(), CharacterASC);
}

void AMonsterController::TargetUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	ACharacterBase* PlayerCharacter = Cast<ACharacterBase>(Actor);
	if (IsValid(PlayerCharacter))
		PlayerCharacter->SetTargeting(GetPawn());

	if (Stimulus.WasSuccessfullySensed())
	{
		if (!mTargets.Contains(Actor))
			mTargets.Add(Actor);
	}
	else
		Blackboard->SetValueAsObject(TEXT("Target"), nullptr);
}
