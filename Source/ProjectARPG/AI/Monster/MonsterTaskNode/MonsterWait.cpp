// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterWait.h"

UMonsterWait::UMonsterWait()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UMonsterWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("CriticalHit")))
	{
		AMonsterBase* MB = OwnerComp.GetOwner<AMonsterController>()->GetPawn<AMonsterBase>();
		if (!IsValid(MB))
			return EBTNodeResult::Failed;

		UMonsterAnimInstance* MAI = Cast<UMonsterAnimInstance>(MB->GetMesh()->GetAnimInstance());
		if (!IsValid(MAI))
			return EBTNodeResult::Failed;

		MAI->StopAllMontages(0.1);
		MAI->SetIsCriticalHit(true);
	}

	return EBTNodeResult::InProgress;
}

void UMonsterWait::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	mWaitTime += DeltaSeconds;
	if (mWaitTime > mWaitTimer)
	{
		mWaitTime = 0;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("CriticalHit")))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
}

void UMonsterWait::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
