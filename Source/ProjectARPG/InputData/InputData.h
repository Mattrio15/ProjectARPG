// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "InputMappingContext.h"
#include "UObject/NoExportTypes.h"
#include "InputData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UInputData : public UObject
{
	GENERATED_BODY()
	
public:
	UInputData();

public:
	UInputMappingContext* mMappingContext;

	UInputAction* mMove;
	UInputAction* mCameraRotation;
	UInputAction* mCharacterChange;
	UInputAction* mDodge;
	UInputAction* mAttack;
	UInputAction* mSkill;
	UInputAction* mUltimate;
	UInputAction* mInventory;
	UInputAction* mQuickSlot_1;
	UInputAction* mQuickSlot_2;
	UInputAction* mQuickSlot_3;
	UInputAction* mPause;


};
