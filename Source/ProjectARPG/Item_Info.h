#pragma once

#include "Item_Info.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItemInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 ItemCount;

};
