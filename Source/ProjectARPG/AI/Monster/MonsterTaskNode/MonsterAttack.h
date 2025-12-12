// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Monster_Task_Info.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MonsterAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UMonsterAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMonsterAttack();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);

};
