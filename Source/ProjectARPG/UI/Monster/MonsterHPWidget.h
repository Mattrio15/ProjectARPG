// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UI_Info.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UMonsterHPWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UProgressBar* mHPBar;
	UProgressBar* mElementalBar;
	UTextBlock* mText_Damage;
	
protected:
	virtual void NativeOnInitialized();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	void SetHPBar(float A);
	void SetElementalBar(float A);
	void SetElementalTexture(UTexture* A);
	void SetTextDamage(int32 Damage);

};
