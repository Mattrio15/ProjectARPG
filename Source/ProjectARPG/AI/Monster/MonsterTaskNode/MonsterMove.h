// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Monster_Task_Info.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MonsterMove.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UMonsterMove : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMonsterMove();

protected:
	AMonsterController* mMonsterController;
	AMonsterBase* mMonster;
	AActor* mTarget;

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);

};
