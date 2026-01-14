// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameWidget.h"
#include "Algo/RandomShuffle.h"

void UMiniGameWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	mRestart = Cast<UButton>(GetWidgetFromName(TEXT("Button_ReStart")));

	mCard00 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_00")));
	mCard01 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_01")));
	mCard02 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_02")));
	mCard03 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_03")));
	mCard04 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_04")));
	mCard05 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_05")));
	mCard06 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_06")));
	mCard07 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_07")));
	mCard08 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_08")));
	mCard09 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_09")));
	mCard10 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_10")));
	mCard11 = Cast<UCardWidget>(GetWidgetFromName(TEXT("WB_Card_11")));

	mCardArray.Add(mCard00);
	mCardArray.Add(mCard01);
	mCardArray.Add(mCard02);
	mCardArray.Add(mCard03);
	mCardArray.Add(mCard04);
	mCardArray.Add(mCard05);
	mCardArray.Add(mCard06);
	mCardArray.Add(mCard07);
	mCardArray.Add(mCard08);
	mCardArray.Add(mCard09);
	mCardArray.Add(mCard10);
	mCardArray.Add(mCard11);

}

void UMiniGameWidget::NativeConstruct()
{
	mRestart->OnClicked.AddDynamic(this, &UMiniGameWidget::ButtonRestartClick);

	TArray<int32> Index;
	for (int32 i = 0; i < 6; ++i)
	{
		Index.Add(i);
		Index.Add(i);
	}
	Algo::RandomShuffle(Index);
	for (int32 i = 0; i < Index.Num(); ++i)
	{
		if (IsValid(mCardArray[i]))
		{
			mCardArray[i]->SetCardIndex(Index[i]);
			mCardArray[i]->SetMiniGameWidget(this);
		}
		else
			Test(TEXT("Card Is InValid!"));
	}

}

void UMiniGameWidget::ButtonRestartClick()
{
	for (int32 i = 0; i < 12; ++i)
	{
		if (mCardArray[i]->GetCardTurn())
			mCardArray[i]->PlayCardAnimation();
	}

}

void UMiniGameWidget::SetSecondCard(UCardWidget* Card)
{
	if (mFirstCard->GetCardIndex() == Card->GetCardIndex())
	{
		mFirstCard->PlayEffect();
		Card->PlayEffect();

		mFirstCard->SetCardTurn(true);
		Card->SetCardTurn(true);
	}
	else
	{
		mFirstCard->PlayCardAnimation();
		Card->PlayCardAnimation();

		mFirstCard->SetCardTurn(false);
		Card->SetCardTurn(false);
	}

	mFirstCard = nullptr;
	mSecondCard = nullptr;
}
