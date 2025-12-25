// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCMove.h"
#include "AIController.h"

UNPCMove::UNPCMove()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UNPCMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	mTreeComponent = &OwnerComp;

	FVector Start = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();

	FVector End = FRotator(0, FMath::RandRange(-180, 180), 0).Vector() * 1000;
	
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("WalkerDir"), Start + End);

	float Timer = FMath::RandRange(mTimer - mRandom, mTimer + mRandom);
	GetWorld()->GetTimerManager().SetTimer(mFinishTimer, this, &UNPCMove::FinishTask, Timer);

	return EBTNodeResult::InProgress;
}

void UNPCMove::FinishTask()
{
	FinishLatentTask(*mTreeComponent, EBTNodeResult::Succeeded);
}
