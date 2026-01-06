// Fill out your copyright notice in the Description page of Project Settings.


#include "Entry_Inventory.h"
#include "ItemDataAsset.h"
#include "CharacterInventory.h"
#include "../../Item_Info.h"
#include "../../Character/ItemComponent.h"
#include "../../Character/CharacterState.h"
#include "CharacterHPWidget.h"
#include "Components/CanvasPanelSlot.h"

void UEntry_Inventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	mPC = GetOwningPlayer();

	mImage_Item = Cast<UImage>(GetWidgetFromName(TEXT("Image_Item")));
	mDescPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("DescPanel")));
	mItemName = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemName")));
	mItemDesc = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemDesc")));
	mItemSelectButton = Cast<UButton>(GetWidgetFromName(TEXT("ItemSelectButton")));
	mCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CountText")));

	mItemSelectButton->OnClicked.AddDynamic(this, &UEntry_Inventory::ItemSelect);
}

void UEntry_Inventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (mIsHovered)
	{
		if (!IsValid(mPC))
			return;
	
		float X = 0, Y = 0;
		mPC->GetMousePosition(X, Y);
		FVector2D Pos = FVector2D(X, Y) - MyGeometry.GetAbsolutePosition();
		mDescPanel->SetRenderTranslation(Pos);
	}
}

void UEntry_Inventory::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UItemDataAsset* Item = Cast<UItemDataAsset>(ListItemObject);
	if (IsValid(Item))
	{
		mImage_Item->SetBrushFromTexture(Item->GetImage());
		mItemName->SetText(FText::FromName(Item->GetItemName()));
		mItemDesc->SetText(Item->GetDescribe());
		ACharacterState* CS = mPC->GetPlayerState<ACharacterState>();
		if (IsValid(CS))
		{
			UItemComponent* IC = CS->GetItemComponent();
			if (IsValid(IC))
			{
				FInventoryItemInfo Info = IC->GetInventoryItemInfo(Item);
				mCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Info.ItemCount)));
			}
		}
	}
}

void UEntry_Inventory::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	mDescPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
	mIsHovered = true;
}

void UEntry_Inventory::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	mDescPanel->SetVisibility(ESlateVisibility::Hidden);
	mIsHovered = false;
}

void UEntry_Inventory::ItemSelect()
{
	ACharacterState* CS = GetOwningPlayer()->GetPlayerState<ACharacterState>();
	if (CS)
	{
		UCharacterHPWidget* MainWidget = CS->GetHPWidget(); // 메인 위젯
		UItemDataAsset* Item = GetListItem<UItemDataAsset>(); // 아이템 데이터 애셋
		if (IsValid(MainWidget) && IsValid(Item))
		{
			if (mClick) // 더블 클릭 유무
			{
				UItemComponent* IC = CS->GetItemComponent(); // 아이템 컴포넌트
				if (IsValid(IC))
					IC->UseItem(Item); // 사용

				MainWidget->SetDragWidget(nullptr, false); // 드래그 취소

				mClick = false;
				return;
			}
			MainWidget->SetDragWidget(Item, true); // 드래그 시작
		}
	}

	mClick = true;
	GetWorld()->GetTimerManager().SetTimer(mClickTimer, this, &UEntry_Inventory::DoubleClickTimer, 0.5);
}

void UEntry_Inventory::DoubleClickTimer()
{
	mClick = false;
}
