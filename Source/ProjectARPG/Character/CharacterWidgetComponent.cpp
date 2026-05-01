// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterWidgetComponent.h"
#include "CharacterState.h"
#include "CharacterController.h"

#include "../UI/Character/CharacterHPWidget.h"
#include "../UI/Character/CharacterInventory.h"
#include "../UI/Character/CharacterInfoWidget.h"
#include "../UI/MiniGame/MiniGameWidget.h"

UCharacterWidgetComponent::UCharacterWidgetComponent()
{
	ClassFinder(UUserWidget, MainWidgetClass, "/Script/UMGEditor.WidgetBlueprint'/Game/UI/Character/WB_MainWidget.WB_MainWidget_C'");
	if (MainWidgetClass.Succeeded())
		mMainWidgetClass = MainWidgetClass.Class;

}

void UCharacterWidgetComponent::InitWidget()
{
	if (IsValid(mMainWidgetClass))
	{
		mMainWidget = CreateWidget<UUserWidget>(GetWorld(), mMainWidgetClass);
		if (IsValid(mMainWidget))
		{
			mHPWidget = Cast<UCharacterHPWidget>(mMainWidget->GetWidgetFromName(TEXT("WB_CharacterHP")));
			mPause = Cast<UUserWidget>(mMainWidget->GetWidgetFromName(TEXT("WB_CharacterPause")));
		}
		if (IsValid(mHPWidget))
		{
			mMainWidget->AddToViewport();

			// mAS_Character->OnHealthChanged.AddUObject(mHPWidget, &UCharacterHPWidget::SetHPBar);
			// mAS_Character->OnManaChanged.AddUObject(mHPWidget, &UCharacterHPWidget::SetMPBar);

			mInventory = Cast<UCharacterInventory>(mHPWidget->GetWidgetFromName(TEXT("WB_CharacterInventory")));
			if (IsValid(mInventory))
			{
				mInventory->SetVisibility(ESlateVisibility::Hidden);
				// mInventory->SetItemComponent(mItemComponent);
				// mItemComponent->SetAbilitySystemComponent(mASC);
				// mItemComponent->SetInventory(mInventory);
				// mItemComponent->SetMainWidget(mHPWidget);
			}

			mInfo = Cast<UCharacterInfoWidget>(mHPWidget->GetWidgetFromName(TEXT("WB_CharacterInfo")));
			if (IsValid(mInfo))
			{
				mInfo->SetVisibility(ESlateVisibility::Hidden);
				// mAS_Character->OnHealthChanged.AddUObject(mInfo, &UCharacterInfoWidget::SetCharacterHPText);
			}

		}
	}
}

void UCharacterWidgetComponent::ShowWidget(ECharacterWidgetType WidgetType, bool A)
{
	UUserWidget* Widget = nullptr;
	switch (WidgetType)
	{
	case ECharacterWidgetType::MainWidget:
		Widget = mMainWidget;
		break;
	case ECharacterWidgetType::FKey:
		if (IsValid(mHPWidget))
			mHPWidget->ShowFKey(A);
		break;
	case ECharacterWidgetType::Inventory:
		Widget = mInventory;
		break;
	case ECharacterWidgetType::Info:
		Widget = mInfo;
		break;
	case ECharacterWidgetType::Pause:
		Widget = mPause;
		break;
	}

	if (!IsValid(Widget))
		return;

	if (A)
	{
		Widget->SetVisibility(ESlateVisibility::Visible);
		if (WidgetType == ECharacterWidgetType::Pause)
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true);

			FInputModeUIOnly Mode;
			mController->SetInputMode(Mode);
			mController->bShowMouseCursor = true;
		}
	}
	else
		Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
}

bool UCharacterWidgetComponent::SetState(ACharacterState* State)
{
	mState = State;
	if (IsValid(mState))
		return true;

	return false;
}

bool UCharacterWidgetComponent::SetController(ACharacterController* Controller)
{
	mController = Controller;
	if (IsValid(mController))
		return true;

	return false;
}
