#pragma once

#include "Engine.h"
#include "EngineMinimal.h"

#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

#include "Engine/DamageEvents.h"

#include "Components/WidgetComponent.h"

#define MyObject(Object, Name) CreateDefaultSubobject<Object>(TEXT(Name))
#define ObjectFinder(Object, Name, Path) static ConstructorHelpers::FObjectFinder<Object> Name(TEXT(Path));

void Test(FString T, float Second = 10, FColor Color = FColor::Red);

void Log(FString T);
