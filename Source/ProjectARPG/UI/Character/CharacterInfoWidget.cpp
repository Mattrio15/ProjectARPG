// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInfoWidget.h"
#include "../../Character/CharacterState.h"

void UCharacterInfoWidget::NativeOnInitialized()
{
}

void UCharacterInfoWidget::NativeConstruct()
{
}

void UCharacterInfoWidget::SetCharacterInfoText(FName Name)
{
	ACharacterState* CS = GetOwningPlayer()->GetPlayerState<ACharacterState>();
	if (!IsValid(CS))
		return;
	FCharacterInfo CI = CS->GetCharacterInfo();
	Text_Name->SetText(FText::FromName(Name));
	int32 HP = CI.CharacterInfo[FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.Character.Health"))];
	Text_HP->SetText(FText::FromString(FString::Printf(TEXT("%d"), HP)));
	int32 Attack = CI.CharacterInfo[FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.Character.Attack"))];
	Text_Attack->SetText(FText::FromString(FString::Printf(TEXT("%d"), Attack)));
	int32 Defense = CI.CharacterInfo[FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.Character.Defense"))];
	Text_Defense->SetText(FText::FromString(FString::Printf(TEXT("%d"), Defense)));
	int32 MoveSpeed = CI.CharacterInfo[FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.Character.MoveSpeed"))];
	Text_MoveSpeed->SetText(FText::FromString(FString::Printf(TEXT("%d"), MoveSpeed)));
	int32 CriticalChance = CI.CharacterInfo[FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.Character.CriticalChance"))] * 100;
	Text_CritChance->SetText(FText::FromString(FString::Printf(TEXT("%d"), CriticalChance)));
	float CriticalRate = CI.CharacterInfo[FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.Character.CriticalRate"))];
	Text_CritRate->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), CriticalRate)));
	float SkillRate = CI.CharacterInfo[FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.Character.SkillRate"))];
	Text_SkillRate->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), SkillRate)));
}
