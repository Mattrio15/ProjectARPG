// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../AI_Info.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "NPCMove.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UNPCMove : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UNPCMove();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mTimer = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mRandom = 0;

	FTimerHandle mFinishTimer;
	UBehaviorTreeComponent* mTreeComponent;

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	void FinishTask();

};
