// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UI_Info.h"
#include "CardWidget.h"
#include "Blueprint/UserWidget.h"
#include "MiniGameWidget.generated.h"

class CardWidget;

UCLASS()
class PROJECTARPG_API UMiniGameWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UButton* mButtonReset;
	UButton* mButtonClose;

	UCardWidget* mCard00;
	UCardWidget* mCard01;
	UCardWidget* mCard02;
	UCardWidget* mCard03;
	UCardWidget* mCard04;
	UCardWidget* mCard05;
	UCardWidget* mCard06;
	UCardWidget* mCard07;
	UCardWidget* mCard08;
	UCardWidget* mCard09;
	UCardWidget* mCard10;
	UCardWidget* mCard11;

	TArray<UCardWidget*> mCardArray;

	UCardWidget* mFirstCard;
	UCardWidget* mSecondCard;

protected:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();

	UFUNCTION()
	void ButtonRestartClick();
	UFUNCTION()
	void ButtonCloseClick();

	UFUNCTION(BlueprintCallable)
	void ShuffleCard();

	UFUNCTION(BlueprintCallable)
	void SetCardEnable(bool A);

public:
	UCardWidget* GetFirstCard() { return mFirstCard; }
	UCardWidget* GetSecondCard() { return mSecondCard; }

	void SetFirstCard(UCardWidget* Card);
	void SetSecondCard(UCardWidget* Card);



};
