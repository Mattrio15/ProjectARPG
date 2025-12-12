// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemComponent.h"

void UItemComponent::AddItem(UItemDataAsset* Item)
{
	if (!mInventoryItems.Contains(Item))
	{
		FInventoryItemInfo Info;
		Info.ItemCount = 1;
		mInventoryItems.Add(Item, Info);
	}
	else
		mInventoryItems[Item].ItemCount += 1;

	mInventory->UpdateInventory(Item);
	for (int32 i = 0; i < 3; ++i)
	{
		if (IsValid(mQuickSlotItem[i]))
			mMainWidget->SetItemCount(i, mInventoryItems[mQuickSlotItem[i]].ItemCount);
	}
}

void UItemComponent::RemoveItem(UItemDataAsset* Item)
{
	if (!mInventoryItems.Contains(Item))
		return;

	mInventoryItems[Item].ItemCount -= 1;

	for (int32 i = 0; i < 3; ++i)
	{
		if (IsValid(mQuickSlotItem[i])) // 퀵 슬롯 업데이트
			mMainWidget->SetItemCount(i, mInventoryItems[mQuickSlotItem[i]].ItemCount);
	}

	if (mInventoryItems[Item].ItemCount <= 0) // 아이템이 없다면
	{
		mInventoryItems.Remove(Item); // 아이템 제거
		for (int32 i = 0; i < 3; ++i)
		{
			if (mQuickSlotItem[i] == Item)
				mQuickSlotItem[i] = nullptr; // 퀵 슬롯 아이템 제거
		}
	}
	mInventory->UpdateInventory(Item); // 인벤토리 업데이트
}

FInventoryItemInfo UItemComponent::GetInventoryItemInfo(UItemDataAsset* ItemDataAsset)
{
	if (mInventoryItems.Contains(ItemDataAsset))
		return mInventoryItems[ItemDataAsset];

	FInventoryItemInfo Info;
	Info.ItemCount = -1;

	return Info;
}

void UItemComponent::UseItem(UItemDataAsset* Item)
{
	if (!IsValid(Item))
		return;

	UGameplayEffect* GE = Item->GetGE()->GetDefaultObject<UGameplayEffect>();
	if (IsValid(GE))
	{
		FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
		mASC->ApplyGameplayEffectToSelf(GE, 1, Context);
	}

	RemoveItem(Item);
}

void UItemComponent::UseQuickSlotItem(int32 Index)
{
	if (IsValid(mQuickSlotItem[Index]))
		UseItem(mQuickSlotItem[Index]);
}

void UItemComponent::SetQuickSlot(UItemDataAsset* Item, int32 Index)
{
	mQuickSlotItem[Index] = Item;
	mMainWidget->SetItemCount(Index, mInventoryItems[Item].ItemCount);
}
