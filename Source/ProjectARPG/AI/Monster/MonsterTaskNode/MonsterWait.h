// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Monster_Task_Info.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MonsterWait.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UMonsterWait : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMonsterWait();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mWaitTimer = 1;
	float mWaitTime = 0;


public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);

};
