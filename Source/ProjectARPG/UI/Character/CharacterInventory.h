// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UI_Info.h"
#include "../../Item_Info.h"
#include "ItemDataAsset.h"
#include "Blueprint/UserWidget.h"
#include "CharacterInventory.generated.h"

class UItemComponent;

UCLASS()
class PROJECTARPG_API UCharacterInventory : public UUserWidget
{
	GENERATED_BODY()

public:
	UButton* mButton_Close;
	UTileView* mTileView_Inventory;
	UItemComponent* mItemComponent;
	
protected:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

protected:
	UFUNCTION()
	void InventoryClose();

public:
	void SetItemComponent(UItemComponent* ItemComponent) { mItemComponent = ItemComponent; }
	void UpdateInventory(UItemDataAsset* Item);

	FInventoryItemInfo GetInventoryItemInfo(UItemDataAsset* Item);
	
};
