// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UI_Info.h"
#include "../../Item_Info.h"
#include "ItemDataAsset.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UQuickSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UButton* mQuickSlot_1;
	UButton* mQuickSlot_2;
	UButton* mQuickSlot_3;

	UImage* mImage_Slot_1;
	UImage* mImage_Slot_2;
	UImage* mImage_Slot_3;

	UTextBlock* mItemCount_1;
	UTextBlock* mItemCount_2;
	UTextBlock* mItemCount_3;

	UItemDataAsset* mItem;

	bool mSlotClick_1 = false;
	bool mSlotClick_2 = false;
	bool mSlotClick_3 = false;

	TArray<UImage*> mImage_Slot;
	TArray<bool> mSlotClick;
	TArray<UTextBlock*> mItemCount;
	TArray<UTexture2D*> mSlotTexture = { nullptr, nullptr, nullptr };

protected:
	virtual void NativeOnInitialized();

protected:
	void SetHovered(int32 Index);
	void SetUnHovered(int32 Index);
	void SetSlot(int32 Index);

	UFUNCTION()
	void QuickSlot_1_Hovered() { SetHovered(0); }
	UFUNCTION()
	void QuickSlot_2_Hovered() { SetHovered(1); }
	UFUNCTION()
	void QuickSlot_3_Hovered() { SetHovered(2); }
	UFUNCTION()
	void QuickSlot_1_UnHovered() { SetUnHovered(0); }
	UFUNCTION()
	void QuickSlot_2_UnHovered() { SetUnHovered(1); }
	UFUNCTION()
	void QuickSlot_3_UnHovered() { SetUnHovered(2); }
	UFUNCTION()
	void SetSlot_1() { SetSlot(0); }
	UFUNCTION()
	void SetSlot_2() { SetSlot(1); }
	UFUNCTION()
	void SetSlot_3() { SetSlot(2); }


public:
	void SetItem(UItemDataAsset* Item) { mItem = Item; }
	void SetItemCount(int32 Index, int32 Count);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayQuickSlotAnimation(int32 Index);

};
