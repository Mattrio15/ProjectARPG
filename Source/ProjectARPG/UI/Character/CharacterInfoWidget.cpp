// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInfoWidget.h"
#include "../../MyType.h"
#include "../../Character/CharacterState.h"
#include "../../GAS/AttributeDataAsset.h"

void UCharacterInfoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UCharacterInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Close->OnClicked.AddDynamic(this, &UCharacterInfoWidget::CloseStatus);

}

FText UCharacterInfoWidget::ChangeTextFromFloat(float A)
{
	FText Text = FText::FromString(FString::Printf(TEXT("%.0f"), A));
	return Text;
}

void UCharacterInfoWidget::CloseStatus()
{
	ACharacterState* CS = GetOwningPlayer()->GetPlayerState<ACharacterState>();
	if (IsValid(CS))
		CS->ShowWidget(ECharacterWidgetType::Info, false);
}

void UCharacterInfoWidget::SetCharacterHPText(float HP)
{
	ACharacterState* CS = GetOwningPlayer()->GetPlayerState<ACharacterState>();
	if (!IsValid(CS))
		return;
	FCharacterInfo CI = CS->GetCharacterInfo();
	int32 RealHP = CI.CharacterInfo[FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.Character.Health"))] * HP;
	Text_HP->SetText(FText::FromString(FString::Printf(TEXT("%d"), RealHP)));

}

void UCharacterInfoWidget::SetInfoText(FName Name)
{
	ACharacterState* CS = GetOwningPlayer()->GetPlayerState<ACharacterState>();
	if (!IsValid(CS))
		return;
	TMap CI = CS->GetCharacterInfo().CharacterInfo;
	UAttributeDataAsset* ADA = CS->GetAttributeTag();
	Text_Name->SetText(FText::FromName(CS->GetCharacterName()));

	Text_HP->SetText(ChangeTextFromFloat(CI[ADA->mHealthTag]));
	Text_Attack->SetText(ChangeTextFromFloat(CI[ADA->mAttackTag]));
	Text_Defense->SetText(ChangeTextFromFloat(CI[ADA->mDefenseTag]));
	Text_MoveSpeed->SetText(ChangeTextFromFloat(CI[ADA->mMoveSpeedTag]));

	Text_CritChance->SetText(ChangeTextFromFloat(CI[ADA->mCriticalChanceTag] * 100));
	Text_CritRate->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), CI[ADA->mCriticalRateTag])));
	Text_SkillRate->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), CI[ADA->mSkillRateTag])));

}
