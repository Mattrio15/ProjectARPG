#pragma once
#include "MyType.generated.h"

UENUM()
enum class ECharacterWidgetType :uint8
{
	MainWidget,
	FKey,
	Inventory,
	Info,
	Pause
};