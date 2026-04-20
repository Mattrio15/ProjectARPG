// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UI_Info.h"
#include "Blueprint/UserWidget.h"
#include "CharacterInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UCharacterInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Name;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_HP;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Attack;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Defense;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_MoveSpeed;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_CritChance;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_CritRate;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_SkillRate;

protected:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();

public:
	void SetCharacterInfoText(FName Name);

};
