// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAttack.h"

UMonsterAttack::UMonsterAttack()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UMonsterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("CriticalHit")))
		return EBTNodeResult::Failed;

	AMonsterBase* Monster = OwnerComp.GetOwner<AMonsterController>()->GetPawn<AMonsterBase>();
	if (!IsValid(Monster))
		return EBTNodeResult::Failed;

	UMonsterAnimInstance* MAI = Cast<UMonsterAnimInstance>(Monster->GetMesh()->GetAnimInstance());
	if (!IsValid(MAI))
		return EBTNodeResult::Failed;

	MAI->PlayMonsterAttack();

	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("AttakEnd")))
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("AttackEnd"), false);

	return EBTNodeResult::Succeeded;
}

void UMonsterAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

}

void UMonsterAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

}
