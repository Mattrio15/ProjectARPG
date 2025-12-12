// Fill out your copyright notice in the Description page of Project Settings.


#include "DragWidget.h"

void UDragWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	mImage_Drag = Cast<UImage>(GetWidgetFromName(TEXT("Image_Drag")));
}

void UDragWidget::SetActivate(UItemDataAsset* Item, bool A)
{
	if (A)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
		if (IsValid(Item))
		{
			mImage_Drag->SetBrushFromTexture(Item->GetImage());
			mImage_Drag->SetOpacity(0.75);
		}
	}
	else
		SetVisibility(ESlateVisibility::Hidden);
}
