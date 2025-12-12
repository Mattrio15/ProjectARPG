// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MonsterBase.h"
#include "Rampage.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API ARampage : public AMonsterBase
{
	GENERATED_BODY()

public:
	ARampage();

public:
	virtual void RealAttack();
	
};
