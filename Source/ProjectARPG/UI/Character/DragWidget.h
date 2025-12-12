// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UI_Info.h"
#include "../../Item_Info.h"
#include "ItemDataAsset.h"
#include "Blueprint/UserWidget.h"
#include "DragWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UDragWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UImage* mImage_Drag;

protected:
	virtual void NativeOnInitialized();

public:
	void SetActivate(UItemDataAsset* Item, bool A);
	
};
