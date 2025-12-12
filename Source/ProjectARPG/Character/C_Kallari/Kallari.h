// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../CharacterBase.h"
#include "Kallari.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API AKallari : public ACharacterBase
{
	GENERATED_BODY()

public:
	AKallari();

protected:
	virtual void DisappearAnimation();

	virtual void ShortAttack();
	virtual void DodgeAttack();

	virtual void RealAttack(int32 Index);
	virtual void RealDodgeAttack();

};
