// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlotWidget.h"
#include "CharacterHPWidget.h"
#include "../../Character/CharacterState.h"
#include "../../Character/ItemComponent.h"

void UQuickSlotWidget::NativeOnInitialized()
{
	mQuickSlot_1 = Cast<UButton>(GetWidgetFromName(TEXT("QuickSlot_1")));
	mQuickSlot_2 = Cast<UButton>(GetWidgetFromName(TEXT("QuickSlot_2")));
	mQuickSlot_3 = Cast<UButton>(GetWidgetFromName(TEXT("QuickSlot_3")));

	mImage_Slot_1 = Cast<UImage>(GetWidgetFromName(TEXT("Image_Slot_1")));
	mImage_Slot_2 = Cast<UImage>(GetWidgetFromName(TEXT("Image_Slot_2")));
	mImage_Slot_3 = Cast<UImage>(GetWidgetFromName(TEXT("Image_Slot_3")));

	mItemCount_1 = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemCount_1")));
	mItemCount_2 = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemCount_2")));
	mItemCount_3 = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemCount_3")));

	if (IsValid(mQuickSlot_1) && IsValid(mQuickSlot_2) && IsValid(mQuickSlot_3))
	{
		mQuickSlot_1->OnHovered.AddDynamic(this, &UQuickSlotWidget::QuickSlot_1_Hovered);
		mQuickSlot_2->OnHovered.AddDynamic(this, &UQuickSlotWidget::QuickSlot_2_Hovered);
		mQuickSlot_3->OnHovered.AddDynamic(this, &UQuickSlotWidget::QuickSlot_3_Hovered);

		mQuickSlot_1->OnUnhovered.AddDynamic(this, &UQuickSlotWidget::QuickSlot_1_UnHovered);
		mQuickSlot_2->OnUnhovered.AddDynamic(this, &UQuickSlotWidget::QuickSlot_2_UnHovered);
		mQuickSlot_3->OnUnhovered.AddDynamic(this, &UQuickSlotWidget::QuickSlot_3_UnHovered);

		mQuickSlot_1->OnClicked.AddDynamic(this, &UQuickSlotWidget::SetSlot_1);
		mQuickSlot_2->OnClicked.AddDynamic(this, &UQuickSlotWidget::SetSlot_2);
		mQuickSlot_3->OnClicked.AddDynamic(this, &UQuickSlotWidget::SetSlot_3);
	}

	mImage_Slot.Add(mImage_Slot_1);
	mImage_Slot.Add(mImage_Slot_2);
	mImage_Slot.Add(mImage_Slot_3);

	mSlotClick.Add(mSlotClick_1);
	mSlotClick.Add(mSlotClick_2);
	mSlotClick.Add(mSlotClick_3);

	mItemCount.Add(mItemCount_1);
	mItemCount.Add(mItemCount_2);
	mItemCount.Add(mItemCount_3);
}

void UQuickSlotWidget::SetHovered(int32 Index)
{
	if (IsValid(mItem))
	{
		mImage_Slot[Index]->SetBrushFromTexture(mItem->GetImage());
		mImage_Slot[Index]->SetOpacity(0.85);
	}
}

void UQuickSlotWidget::SetUnHovered(int32 Index)
{
	if (!mSlotClick[Index])
		mImage_Slot[Index]->SetOpacity(0);
	else
		mImage_Slot[Index]->SetOpacity(1);
}

void UQuickSlotWidget::SetSlot(int32 Index)
{
	if (IsValid(mItem))
	{
		mImage_Slot[Index]->SetBrushFromTexture(mItem->GetImage()); // 이미지 설정
		mImage_Slot[Index]->SetOpacity(1);							// 투명 끄기
		mSlotClick[Index] = true;									// 슬롯에 아이엠이 있음
		
		ACharacterState* CS = GetOwningPlayer()->GetPlayerState<ACharacterState>();
		if (IsValid(CS))
		{
			UItemComponent* IC = CS->GetItemComponent();
			if (IsValid(IC))
				IC->SetQuickSlot(mItem, Index);		// 아이템 컴포넌트에 몇 번째 칸에 아이템이 있다고 알려줌

			UCharacterHPWidget* MainWidget = CS->GetMainWidget();
			if (IsValid(MainWidget))
				MainWidget->SetDragWidget(nullptr, false);			// 드래그 위젯은 이제 불필요
		}
	}
}

void UQuickSlotWidget::SetItemCount(int32 Index, int32 Count)
{
	mItemCount[Index]->SetText(FText::FromString(FString::Printf(TEXT("%d"), Count)));
}
