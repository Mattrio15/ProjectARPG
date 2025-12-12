// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterHPWidget.h"

void UMonsterHPWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	mHPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));
	mElementalBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ElementalBar")));
	mText_Damage = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_Damage")));
}

void UMonsterHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UMonsterHPWidget::SetHPBar(float A)
{
	mHPBar->SetPercent(A);
}

void UMonsterHPWidget::SetElementalBar(float A)
{
	mElementalBar->SetPercent(A / 100);
}

void UMonsterHPWidget::SetElementalTexture(UTexture* A)
{
	if (IsValid(A))
	{
		FProgressBarStyle Style = mElementalBar->GetWidgetStyle();
		FSlateImageBrush Brush1 = FSlateImageBrush(A, FVector2d(40, 40), FColor(255, 255, 255, 80));
		Style.SetBackgroundImage(Brush1);
		FSlateImageBrush Brush2 = FSlateImageBrush(A, FVector2d(40, 40));
		Style.SetFillImage(Brush2);
		mElementalBar->SetWidgetStyle(Style);
	}
	else
		mElementalBar->SetFillColorAndOpacity(FColor::Black);
}

void UMonsterHPWidget::SetTextDamage(int32 Damage)
{
	mText_Damage->SetText(FText::FromString(FString::Printf(TEXT("%d"), Damage)));
}
