// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UI_Info.h"
#include "Blueprint/UserWidget.h"
#include "CardWidget.generated.h"

class UMiniGameWidget;

UCLASS()
class PROJECTARPG_API UCardWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	int32 mCardIndex = 0;
	UImage* mCardImage;
	UButton* mCardButton;

	UMiniGameWidget* mMiniGameWidget;

	bool mCardTurn = false;

protected:
	virtual void NativeOnInitialized();

	UFUNCTION()
	void CardClick();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void PlayCardAnimation();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayEffect();

public:
	int32 GetCardIndex() { return mCardIndex; }
	bool GetCardTurn() { return mCardTurn; }

	void SetCardIndex(int32 Index);
	void SetMiniGameWidget(UMiniGameWidget* Widget) { mMiniGameWidget = Widget; }
	void SetCardTurn(bool A) { mCardTurn = A; }

};
