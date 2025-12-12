// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterMove.h"

UMonsterMove::UMonsterMove()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UMonsterMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	mMonsterController = OwnerComp.GetOwner<AMonsterController>();
	if (!IsValid(mMonsterController))
		return EBTNodeResult::Failed;

	mMonster = mMonsterController->GetPawn<AMonsterBase>();
	if (!IsValid(mMonster))
		return EBTNodeResult::Failed;

	mTarget = Cast<AActor>(mMonsterController->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (!IsValid(mTarget))
		return EBTNodeResult::Failed;

	return EBTNodeResult::InProgress;
}

void UMonsterMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	
	FVector Dir = mTarget->GetActorLocation() - mMonster->GetActorLocation();
	Dir -= FVector(0, 0, Dir.Z);

	mMonster->AddMovementInput(Dir, 1);
	mMonster->SetActorRelativeRotation(Dir.Rotation());

	if (Dir.Length() < 400)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (mMonsterController->GetBlackboardComponent()->GetValueAsBool(TEXT("CriticalHit")))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
}

void UMonsterMove::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

}
