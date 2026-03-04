// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../AI_Info.h"
#include "GenericTeamAgentInterface.h"
#include "MonsterController.h"
#include "../../GAS/AttributeSet/MonsterAttributeSet.h"
#include "../../GAS/AttributeDataAsset.h"
#include "../../GAS/AttributeSet/ElementalDataAsset.h"
#include "GameFramework/Character.h"
#include "MonsterBase.generated.h"

USTRUCT(BlueprintType)
struct FMonsterInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, float> MonsterInfo;
};

UCLASS()
class PROJECTARPG_API AMonsterBase : public ACharacter, public IGenericTeamAgentInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMonsterBase();

protected:
	FGenericTeamId mTeamId = FGenericTeamId(2);
	UBehaviorTree* mTree;

	FName mName;

	UAttributeDataAsset* mADA;
	UAbilitySystemComponent* mASC;
	UPROPERTY(EditDefaultsOnly)
	UMonsterAttributeSet* mMAS;
	
	static UDataTable* mMonsterInfoTable;

	TSubclassOf<UGameplayEffect> mGE_MonsterInit;
	TSubclassOf<UGameplayEffect> mGE_MonsterDamage;
	TSubclassOf<UGameplayEffect> mGE_ElementalOver;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* mMonsterHPWidget;

	TArray<UMaterialInstanceDynamic*> mMIDArray;
	float mDissolveValue = -1;
	FTimerHandle mDissolveTimer;

	UElementalDataAsset* mEDA;

	APlayerController* mPlayerController = nullptr;

public:
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) {}
	virtual FGenericTeamId GetGenericTeamId() const { return mTeamId; }
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	virtual void PossessedBy(AController* NewController) override;

public:
	UBehaviorTree* GetTree() { return mTree; }

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	virtual void RealAttack(){}

public:
	void CriticalHit(FVector Dir);
	void SetIsHit(bool A);

	void Death(FVector Pos);
	void SetElementalTexture();

	void PlayKnockback(FVector Start);

protected:
	void Dissolve();
	void SetMoveSpeed(float Speed);
	void PlayGE_ElementalOver(UAbilitySystemComponent* Source);

};
