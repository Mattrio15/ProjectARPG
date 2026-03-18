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

/*
* TM : TMap
* TA : TArray
* 
* DA : Data Asset
* DT : Data Table
* 
* AS : Attribute Set
* GA : Gameplay Ability
* GE : Gameplay Effect
* GT : Gameplay Tag
* 
* NC : Niagara Component
* NS : Niagara System
* PS : Particle System
* SB : Sound Base
* 
*/ 

#define MyObject(Object, Name) CreateDefaultSubobject<Object>(TEXT(Name))
#define ObjectFinder(Object, Name, Path) static ConstructorHelpers::FObjectFinder<Object> Name(TEXT(Path));
#define ClassFinder(Object, Name, Path) static ConstructorHelpers::FClassFinder<Object> Name(TEXT(Path));

#define SoftPath(Object, Path) TSoftObjectPtr<Object>(FSoftObjectPath(TEXT(Path)));

void Test(FString T, float Second = 10, FColor Color = FColor::Red);

void Log(FString T);
