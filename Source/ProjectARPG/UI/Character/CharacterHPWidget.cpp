// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHPWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "../../Character/CharacterState.h"

void UCharacterHPWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	mHPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));
	mMPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("MPBar")));

	mDragWidget = Cast<UDragWidget>(GetWidgetFromName(TEXT("WB_Drag")));
	mQuickSlotWidget = Cast<UQuickSlotWidget>(GetWidgetFromName(TEXT("WB_QuickSlot")));

	mImage_CharacterFace = Cast<UImage>(GetWidgetFromName(TEXT("Image_CharacterFace")));

}

FReply UCharacterHPWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (mDragWidget->GetVisibility() == ESlateVisibility::HitTestInvisible)
	{
		UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(mDragWidget->Slot);
		FVector2D Pos = InMouseEvent.GetScreenSpacePosition();
		FVector2D Pos2 = InGeometry.AbsoluteToLocal(Pos) - FVector2D(50, 50);
		PanelSlot->SetPosition(Pos2);		
	}

	return Reply;
}

FReply UCharacterHPWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	SetDragWidget(nullptr, false);

	return Reply;
}

void UCharacterHPWidget::SetHPBar(float A)
{
	mHPBar->SetPercent(A);
}

void UCharacterHPWidget::SetMPBar(float A)
{
	mMPBar->SetPercent(A);
}

void UCharacterHPWidget::SetDragWidget(UItemDataAsset* Item, bool A)
{
	mDragItem = Item;					// 드래그하고 있는 아이템
	mDragWidget->SetActivate(Item, A);	// 드래그 위젯 활성화
	mQuickSlotWidget->SetItem(Item);	// 퀵 슬롯에 들어갈 예정인 아이템
	if(!A)
	{
		UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(mDragWidget->Slot);
		PanelSlot->SetPosition(FVector2D(-200, -200));
	}
}

void UCharacterHPWidget::SetItemCount(int32 Index, int32 Count)
{
	mQuickSlotWidget->SetItemCount(Index, Count);
}

void UCharacterHPWidget::PlayQuickSlotAnimation(int32 Index)
{
	if (IsValid(mQuickSlotWidget))
		mQuickSlotWidget->PlayQuickSlotAnimation(Index);
}

void UCharacterHPWidget::SetCharacterFace(UTexture2D* Face)
{
	if (!IsValid(mImage_CharacterFace) || !IsValid(Face))
		return;

	mImage_CharacterFace->SetBrushFromTexture(Face, true);

}
