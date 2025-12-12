// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UI_Info.h"
#include "../../Item_Info.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "Entry_Inventory.generated.h"

class UItemComponent;

UCLASS()
class PROJECTARPG_API UEntry_Inventory : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	TObjectPtr<APlayerController> mPC;

	UImage* mImage_Item;
	UCanvasPanel* mDescPanel;
	UTextBlock* mItemName;
	UTextBlock* mItemDesc;
	UButton* mItemSelectButton;
	UTextBlock* mCountText;

	bool mIsHovered = false;

	bool mDoubleClick = false;
	bool mClick = false;
	FTimerHandle mClickTimer;

protected:
	virtual void NativeOnInitialized();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);

protected:
	UFUNCTION()
	void ItemSelect();
	void DoubleClickTimer();
		
};
