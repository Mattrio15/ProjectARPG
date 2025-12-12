// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "Engine/DataAsset.h"
#include "DA_CharacterGE.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARPG_API UDA_CharacterGE : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "초기화")
	TSubclassOf<UGameplayEffect> mGE_Init;

	UPROPERTY(EditDefaultsOnly, Category = "기본 공격")
	TMap<FName, TSubclassOf<UGameplayEffect>> mGE_Attack;

	UPROPERTY(EditDefaultsOnly, Category = "카운터 공격")
	TMap<FName, TSubclassOf<UGameplayEffect>> mGE_CounterAttack;

	UPROPERTY(EditDefaultsOnly, Category = "스킬")
	TMap<FName, TSubclassOf<UGameplayEffect>> mGE_Skill;

	UPROPERTY(EditDefaultsOnly, Category = "속성 부여")
	TMap<FName, FGameplayTag> mGE_Elemental_Tag;

	
};
