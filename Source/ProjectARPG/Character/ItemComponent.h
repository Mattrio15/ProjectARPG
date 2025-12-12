// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "../Item_Info.h"
#include "../UI/Character/ItemDataAsset.h"
#include "../UI/Character/CharacterInventory.h"
#include "../UI/Character/CharacterHPWidget.h"
#include "UObject/NoExportTypes.h"
#include "ItemComponent.generated.h"

class UCharacterInventory;

UCLASS()
class PROJECTARPG_API UItemComponent : public UObject
{
	GENERATED_BODY()
	
protected:
	UAbilitySystemComponent* mASC;
	UCharacterInventory* mInventory;
	UCharacterHPWidget* mMainWidget;

	UPROPERTY(EditAnywhere)
	TMap<UItemDataAsset*, FInventoryItemInfo> mInventoryItems;

	TArray<UItemDataAsset*> mQuickSlotItem = { nullptr,nullptr,nullptr };

public:
	void SetAbilitySystemComponent(UAbilitySystemComponent* ASC) { mASC = ASC; }
	void SetInventory(UCharacterInventory* Inventory) { mInventory = Inventory; }
	void SetMainWidget(UCharacterHPWidget* MainWidget) { mMainWidget = MainWidget; }

	void AddItem(UItemDataAsset* Item);
	void RemoveItem(UItemDataAsset* Item);
	
	FInventoryItemInfo GetInventoryItemInfo(UItemDataAsset* ItemDataAsset);

	void UseItem(UItemDataAsset* Item);
	void UseQuickSlotItem(int32 Index);

	void SetQuickSlot(UItemDataAsset* Item, int32 Index);
	
};
