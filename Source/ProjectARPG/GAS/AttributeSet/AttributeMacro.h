#pragma once

#include "AttributeSet.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnManaChanged, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMoveSpeedChanged, float);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnElementalChanged, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnElementalOver, UAbilitySystemComponent*);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHitDamage, int32);