// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInventory.h"
#include "../../Character/CharacterState.h"
#include "Entry_Inventory.h"
#include "../../Character/ItemComponent.h"

void UCharacterInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	mButton_Close = Cast<UButton>(GetWidgetFromName(TEXT("Button_Close")));
	mTileView_Inventory = Cast<UTileView>(GetWidgetFromName(TEXT("TileView_Inventory")));

}

void UCharacterInventory::NativeConstruct()
{
	Super::NativeConstruct();

	mButton_Close->OnClicked.AddDynamic(this, &UCharacterInventory::InventoryClose);
}

void UCharacterInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
}

void UCharacterInventory::InventoryClose()
{
	ACharacterState* CS = GetOwningPlayer()->GetPlayerState<ACharacterState>();
	if (IsValid(CS))
		CS->ShowInventory(false);
}

void UCharacterInventory::UpdateInventory(UItemDataAsset* Item)
{
	if (mTileView_Inventory->GetIndexForItem(Item) < 0)
		mTileView_Inventory->AddItem(Item);
	
	FInventoryItemInfo Info = mItemComponent->GetInventoryItemInfo(Item);
	if (Info.ItemCount <= 0)
		mTileView_Inventory->RemoveItem(Item);
	
	mTileView_Inventory->RegenerateAllEntries();
}

FInventoryItemInfo UCharacterInventory::GetInventoryItemInfo(UItemDataAsset* Item)
{
	return mItemComponent->GetInventoryItemInfo(Item);
}
