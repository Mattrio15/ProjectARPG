// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameWidget.h"
#include "Algo/RandomShuffle.h"
#include "../../Character/CharacterState.h"

void UMiniGameWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	mButtonReset = Cast<UButton>(GetWidgetFromName(TEXT("Button_Reset")));
	mButtonClose = Cast<UButton>(GetWidgetFromName(TEXT("Button_Close")));

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
	if(IsValid(mButtonReset))
		mButtonReset->OnClicked.AddDynamic(this, &UMiniGameWidget::ButtonRestartClick);
	if (IsValid(mButtonClose))
		mButtonClose->OnClicked.AddDynamic(this, &UMiniGameWidget::ButtonCloseClick);

	ShuffleCard();

}

void UMiniGameWidget::ButtonRestartClick()
{
	for (int32 i = 0; i < 12; ++i)
	{
		if (!IsValid(mCardArray[i]))
			continue;

		if (mCardArray[i]->GetCardTurn())
		{
			mCardArray[i]->PlayCardAnimation();
			mCardArray[i]->SetCardTurn(false);
		}
	
	}

	SetCardEnable(false);

	if (IsValid(mFirstCard))
		mFirstCard = nullptr;
}

void UMiniGameWidget::ButtonCloseClick()
{
	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC))
		return;

	FInputModeGameOnly Mode;
	PC->SetInputMode(Mode);
	PC->bShowMouseCursor = false;
	
	ACharacterState* CS = PC->GetPlayerState<ACharacterState>();
	if (!IsValid(CS))
		return;

	CS->ShowMainWidget(true);
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	SetVisibility(ESlateVisibility::Hidden);
}

void UMiniGameWidget::ShuffleCard()
{
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

	mCorrectCount = 0;

}

void UMiniGameWidget::SetCardEnable(bool A)
{
	for (int32 i = 0; i < 12; ++i)
	{
		if (IsValid(mCardArray[i]))
			mCardArray[i]->SetIsEnabled(A);
	}
}

void UMiniGameWidget::SetFirstCard(UCardWidget* Card)
{
	mFirstCard = Card;
	Card->SetCardTurn(true);
}

void UMiniGameWidget::SetSecondCard(UCardWidget* Card)
{
	if (mFirstCard->GetCardIndex() == Card->GetCardIndex())
	{
		mFirstCard->PlayEffect();
		Card->PlayEffect();

		mFirstCard->SetCardTurn(true);
		Card->SetCardTurn(true);
		mCorrectCount += 1;

		if (mCorrectCount >= 6)
		{
			ACharacterState* CS = GetOwningPlayer()->GetPlayerState<ACharacterState>();
			if (IsValid(CS))
			{
				UItemDataAsset* HpPotion = LoadObject<UItemDataAsset>(GetWorld(), TEXT("/Script/ProjectARPG.ItemDataAsset'/Game/UI/Item/DA_HealthPotion.DA_HealthPotion'"));
				UItemDataAsset* MpPotion = LoadObject<UItemDataAsset>(GetWorld(), TEXT("/Script/ProjectARPG.ItemDataAsset'/Game/UI/Item/DA_ManaPotion.DA_ManaPotion'"));
				if (IsValid(HpPotion) && IsValid(MpPotion))
				{
					for (int32 i = 0; i < 3; ++i)
					{
						CS->GetItem(HpPotion);
						CS->GetItem(MpPotion);
					}
				}
			}
			ClearMiniGame();
			mCorrectCount = 0;
		}
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
