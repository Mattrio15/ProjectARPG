// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "../../AI/Monster/MonsterBase.h"

UMonsterAttributeSet::UMonsterAttributeSet()
{
}

void UMonsterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attribute = Data.EvaluatedData.Attribute;
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

	if (Attribute == GetHealthAttribute())
	{
		Health.SetCurrentValue(FMath::Clamp(Health.GetCurrentValue(), 0, Health.GetBaseValue()));
		AMonsterBase* Monster = Cast<AMonsterBase>(ASC->GetAvatarActor());
		if (IsValid(Monster))
		{
			if (GetHealth() <= 0)
			{
				AActor* CS = Data.EffectSpec.GetContext().GetInstigator();
				ACharacter* Character = Cast<ACharacter>(Cast<APlayerState>(CS)->GetPawn());
				if (IsValid(Character))
					Monster->Death(Character->GetActorLocation());
			}
			else
				Monster->SetIsHit(true);
		}
	}
	else if (Attribute == GetAttackAttribute())
		Attack.SetCurrentValue(FMath::Max(0, Attack.GetCurrentValue()));
	else if (Attribute == GetDefenseAttribute())
		Defense.SetCurrentValue(FMath::Max(0, Defense.GetCurrentValue()));
	else if (Attribute == GetMoveSpeedAttribute())
		MoveSpeed.SetCurrentValue(FMath::Max(0, MoveSpeed.GetCurrentValue()));
	else if (Attribute == GetElementalRatingAttribute())
	{
		ElementalRating.SetCurrentValue(FMath::Max(0, GetElementalRating()));
		if (GetElementalRating() >= 100)
		{
			UAbilitySystemComponent* SourceASC = Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent();
			OnElementalOver.Broadcast(SourceASC);
		}
	}
	
}

void UMonsterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		OnHealthChanged.Broadcast(Health.GetCurrentValue() / MaxHealth.GetBaseValue());
		int32 Damage = FMath::Floor(OldValue - NewValue);
		OnHitDamage.Broadcast(Damage);
	}
	else if (Attribute == GetMoveSpeedAttribute())
		OnMoveSpeedChanged.Broadcast(MoveSpeed.GetCurrentValue());
	else if (Attribute == GetElementalRatingAttribute())
		OnElementalChanged.Broadcast(GetElementalRating());

}
