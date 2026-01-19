// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../AI_Info.h"
#include "Animation/AnimInstance.h"
#include "NPCAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UNPCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool mTalking = false;

public:
	void SetIsTalking(bool A) { mTalking = A; }
	
};
