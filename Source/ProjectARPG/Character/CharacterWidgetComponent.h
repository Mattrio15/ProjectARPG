// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "../MyType.h"
#include "UObject/NoExportTypes.h"
#include "CharacterWidgetComponent.generated.h"

class ACharacterController;
class ACharacterState;

class UCharacterHPWidget;
class UCharacterInventory;
class UCharacterInfoWidget;
class UMiniGameWidget;

UCLASS()
class PROJECTARPG_API UCharacterWidgetComponent : public UObject
{
	GENERATED_BODY()

public:
	UCharacterWidgetComponent();
	
protected:
	UPROPERTY()
	TObjectPtr<ACharacterState> mState;
	UPROPERTY()
	TObjectPtr<ACharacterController> mController;

	UPROPERTY()
	TSubclassOf<UUserWidget> mMainWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> mMainWidget;
	UPROPERTY()
	TObjectPtr<UCharacterHPWidget> mHPWidget;
	UPROPERTY()
	TObjectPtr<UCharacterInventory> mInventory;
	UPROPERTY()
	TObjectPtr<UCharacterInfoWidget> mInfo;
	UPROPERTY()
	TObjectPtr<UMiniGameWidget> mMiniGame;
	UPROPERTY()
	TObjectPtr<UUserWidget> mPause;

public:
	void InitWidget();

	void ShowWidget(ECharacterWidgetType Widget, bool A);

public:
	bool SetState(ACharacterState* State);
	bool SetController(ACharacterController* Controller);

};
