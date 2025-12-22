// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UI_Info.h"
#include "DragWidget.h"
#include "QuickSlotWidget.h"
#include "ItemDataAsset.h"
#include "Blueprint/UserWidget.h"
#include "CharacterHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UCharacterHPWidget : public UUserWidget
{
	GENERATED_BODY()
		
protected:
	UProgressBar* mHPBar;
	UProgressBar* mMPBar;
	UDragWidget* mDragWidget;
	UQuickSlotWidget* mQuickSlotWidget;
	UItemDataAsset* mDragItem;

	UImage* mImage_CharacterFace;

protected:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);


public:
	void SetHPBar(float A);
	void SetMPBar(float A);

	void SetDragWidget(UItemDataAsset* Item, bool A);
	void SetItemCount(int32 Index, int32 Count);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayButtonAnimation(int32 Index);

	void PlayQuickSlotAnimation(int32 Index);

	void SetCharacterFace(UTexture2D* Face);

};
