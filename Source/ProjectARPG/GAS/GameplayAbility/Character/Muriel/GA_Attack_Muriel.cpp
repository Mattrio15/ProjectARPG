// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Attack_Muriel.h"
#include "../../../../Character/C_Muriel/Muriel.h"
#include "../../../../Character/C_Muriel/Muriel_Attack.h"

void UGA_Attack_Muriel::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AMuriel* Muriel = Cast<AMuriel>(ActorInfo->AvatarActor);

	UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/Character/Muriel/P_Muriel_Primary_Spawn.P_Muriel_Primary_Spawn'"));

	if (!IsValid(PS) || !IsValid(Muriel))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	FVector AttackL = Muriel->GetMesh()->GetSocketLocation(TEXT("AttackL"));
	FVector AttackR = Muriel->GetMesh()->GetSocketLocation(TEXT("AttackR"));
	UGameplayStatics::SpawnEmitterAttached(PS, Muriel->GetMesh(), TEXT(""), AttackL, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition);
	UGameplayStatics::SpawnEmitterAttached(PS, Muriel->GetMesh(), TEXT(""), AttackR, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition);

	FVector End = Muriel->GetEndVector();

	AMuriel_Attack* MA1 = GetWorld()->SpawnActor<AMuriel_Attack>(AMuriel_Attack::StaticClass(), AttackL, (End - AttackL).Rotation() + FRotator(10, 0, 0));
	AMuriel_Attack* MA2 = GetWorld()->SpawnActor<AMuriel_Attack>(AMuriel_Attack::StaticClass(), AttackR, (End - AttackR).Rotation() + FRotator(10, 0, 0));
	if (IsValid(MA1) && IsValid(MA2))
	{
		MA1->SetActorRotation((End - AttackL).Rotation() + FRotator(10, 0, 0));
		MA2->SetActorRotation((End - AttackR).Rotation() + FRotator(10, 0, 0));
		MA1->SetParent(Muriel);
		MA2->SetParent(Muriel);
	}

	USoundBase* SB = LoadObject<USoundBase>(GetWorld(), TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Muriel_Attack.SW_Muriel_Attack'"));
	if (IsValid(SB))
		UGameplayStatics::PlaySound2D(GetWorld(), SB);

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
