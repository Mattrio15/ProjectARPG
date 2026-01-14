// Fill out your copyright notice in the Description page of Project Settings.


#include "CardWidget.h"
#include "MiniGameWidget.h"

void UCardWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	mCardImage = Cast<UImage>(GetWidgetFromName(TEXT("Card_Image")));
	mCardButton = Cast<UButton>(GetWidgetFromName(TEXT("CardButton")));

	mCardButton->OnClicked.AddDynamic(this, &UCardWidget::CardClick);

}

void UCardWidget::CardClick()
{
	if (!IsValid(mMiniGameWidget))
		return;

	if (!IsValid(mMiniGameWidget->GetFirstCard()))
		mMiniGameWidget->SetFirstCard(this);
	else
		mMiniGameWidget->SetSecondCard(this);

}

void UCardWidget::SetCardIndex(int32 Index)
{
	mCardIndex = Index;
	
	UTexture2D* Image = nullptr;
	switch (Index)
	{
	case 0:
		Image = LoadObject<UTexture2D>(GetWorld(), TEXT("/Script/Engine.Texture2D'/Game/UI/Image/Face_Aurora.Face_Aurora'"));
		break;
	case 1:
		Image = LoadObject<UTexture2D>(GetWorld(), TEXT("/Script/Engine.Texture2D'/Game/UI/Image/Face_Kallari.Face_Kallari'"));
		break;
	case 2:
		Image = LoadObject<UTexture2D>(GetWorld(), TEXT("/Script/Engine.Texture2D'/Game/UI/Image/Face_Muriel.Face_Muriel'"));
		break;
	case 3:
		Image = LoadObject<UTexture2D>(GetWorld(), TEXT("/Script/Engine.Texture2D'/Game/UI/Image/Face_Revenant.Face_Revenant'"));
		break;
	case 4:
		Image = LoadObject<UTexture2D>(GetWorld(), TEXT("/Script/Engine.Texture2D'/Game/UI/Image/Face_Terra.Face_Terra'"));
		break;
	case 5:
		Image = LoadObject<UTexture2D>(GetWorld(), TEXT("/Script/Engine.Texture2D'/Game/UI/Image/Face_TwinBlast.Face_TwinBlast'"));
		break;
	}

	if (IsValid(Image))
		mCardImage->SetBrushFromTexture(Image);
}
