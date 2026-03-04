// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterController.h"
#include "CharacterBase.h"
#include "C_Aurora/Aurora.h"
#include "C_Kallari/Kallari.h"
#include "C_Muriel/Muriel.h"
#include "C_Revenant/Revenant.h"
#include "C_Terra/Terra.h"
#include "C_TwinBlast/TwinBlast.h"
#include "../MyGameInstance.h"
#include "../SaveGame/MySaveGame.h"


ACharacterController::ACharacterController()
{
	ObjectFinder(UNiagaraSystem, NS, "/Script/Niagara.NiagaraSystem'/Game/Niagara/NS_Wave.NS_Wave'");
	if (NS.Succeeded())
		mCounterAttackEffect = NS.Object;
}

void ACharacterController::BeginPlay()
{
	Super::BeginPlay();

	mCounterCamera = GetWorld()->SpawnActor<ACounterCamera>(ACounterCamera::StaticClass(), FVector(0, 0, 0), FRotator::ZeroRotator);

	GetWorld()->GetTimerManager().SetTimer(mSpawnTimer, this, &ACharacterController::SpawnCharacter, 0.1, false);

}

void ACharacterController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

void ACharacterController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ACharacterController::SpawnCharacter()
{
	UMyGameInstance* GI = GetGameInstance<UMyGameInstance>();
	if (!IsValid(GI))
		return;
	if (GI->GetIsNewGame())
		return;

	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
		CS->SetSaveGameData(GI->GetSaveGameData());

	GetPawn()->Destroy();

	ACharacterBase* CB = nullptr;
	FSaveGameData SD = GI->GetSaveGameData();
	if (SD.CharacterName == TEXT("Kallari"))
	{
		CB = GetWorld()->SpawnActor<AKallari>(AKallari::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
		mCharacterIndex = 1;
	}
	else if (SD.CharacterName == TEXT("Muriel"))
	{
		CB = GetWorld()->SpawnActor<AMuriel>(AMuriel::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
		mCharacterIndex = 2;
	}
	else if (SD.CharacterName == TEXT("Revenant"))
	{
		CB = GetWorld()->SpawnActor<ARevenant>(ARevenant::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
		mCharacterIndex = 3;
	}
	else if (SD.CharacterName == TEXT("Terra"))
	{
		CB = GetWorld()->SpawnActor<ATerra>(ATerra::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
		mCharacterIndex = 4;
	}
	else if (SD.CharacterName == TEXT("TwinBlast"))
	{
		CB = GetWorld()->SpawnActor<ATwinBlast>(ATwinBlast::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
		mCharacterIndex = 5;
	}
	else
		CB = GetWorld()->SpawnActor<AAurora>(AAurora::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
	
	CB->SetActorLocation(SD.CharacterLocation);
	CB->SetActorRotation(SD.CharacterRotation);
	CB->SetCameraRotation(SD.CameraRotation);
	CB->SetDirYawChange(SD.DirYaw);

	OnPossess(CB);

}

void ACharacterController::CharacterChange(FVector Postion, FRotator CameraRotation, float DirYaw)
{
	OnUnPossess();

	ACharacterBase* MyCharacter = nullptr;
	switch (mCharacterIndex)
	{
	case 0:
		MyCharacter = GetWorld()->SpawnActor<AKallari>(AKallari::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
		break;
	case 1:
		MyCharacter = GetWorld()->SpawnActor<AMuriel>(AMuriel::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
		break;
	case 2:
		MyCharacter = GetWorld()->SpawnActor<ARevenant>(ARevenant::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
		break;
	case 3:
		MyCharacter = GetWorld()->SpawnActor<ATerra>(ATerra::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
		break;
	case 4:
		MyCharacter = GetWorld()->SpawnActor<ATwinBlast>(ATwinBlast::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
		break;
	case 5:
		MyCharacter = GetWorld()->SpawnActor<AAurora>(AAurora::StaticClass(), FVector(0, 0, -10000), FRotator::ZeroRotator);
		break;
	}
	mCharacterIndex += 1;
	mCharacterIndex %= 6;
	if (IsValid(MyCharacter))
	{
		MyCharacter->SetActorLocation(Postion);
		MyCharacter->SetCameraRotation(CameraRotation);
		MyCharacter->SetDirYawChange(DirYaw);
		OnPossess(MyCharacter);

		if (mCounterEnable)
		{
			if (IsValid(mCounterCamera))
			{
				mCounterCamera->SetActorLocation(MyCharacter->GetActorLocation());
				int32 i = FMath::RandRange(0, 1);
				mCounterCamera->SetActorRotation(FRotator(2.5, DirYaw + 45*FMath::Pow(-1.0, i), 0));
				SetViewTargetWithBlend(mCounterCamera, 0.1);
				TSubclassOf<UCameraShakeBase> CSB = LoadClass<UCameraShakeBase>(GetWorld(), 
					TEXT("/Script/Engine.Blueprint'/Game/CameraShake/BC_CounterChange.BC_CounterChange_C'"));
				if (IsValid(CSB))
					UGameplayStatics::PlayWorldCameraShake(GetWorld(), CSB, MyCharacter->GetActorLocation(), 0, 10000);
				mCounterCamera->PostOn(true);
			}
			MyCharacter->CounterChange();
			if (mCounterAttackEffect.IsValid())
			{
				UNiagaraSystem* NS = mCounterAttackEffect.Get();
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS, MyCharacter->GetActorLocation());
			}

			USoundBase* SB = LoadObject<USoundBase>(GetWorld(), TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_Counter.SW_Counter'"));
			if (IsValid(SB))
				UGameplayStatics::PlaySound2D(GetWorld(), SB);
		}
	}
}
