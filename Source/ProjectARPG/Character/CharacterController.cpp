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

	ObjectFinder(USoundBase, SB, "/Script/Engine.SoundWave'/Game/Sound/SW_Counter.SW_Counter'");
	if (SB.Succeeded())
		mCounterSound = SB.Object;

	ClassFinder(UCameraShakeBase, CSB, "/Script/Engine.Blueprint'/Game/CameraShake/BC_CounterChange.BC_CounterChange_C'");
	if (CSB.Succeeded())
		mCounterCameraShake = CSB.Class;

	ObjectFinder(UDataTable, DT, "/Script/Engine.DataTable'/Game/Character/DT_CharacterClass.DT_CharacterClass'");
	if (DT.Succeeded())
		mCharacterClassInfoTable = DT.Object;
}

void ACharacterController::BeginPlay()
{
	Super::BeginPlay();

	mCounterCamera = GetWorld()->SpawnActor<ACounterCamera>(ACounterCamera::StaticClass(), FVector(0, 0, 0), FRotator::ZeroRotator);

	GetWorld()->GetTimerManager().SetTimer(mSpawnTimer, this, &ACharacterController::SpawnCharacter, 0.1, false);

	if (IsValid(mCharacterClassInfoTable))
		mCharacterClassInfoTable->GetAllRows<FCharacterClassInfo>(TEXT(""), mCharacterClassInfoArray);

}

void ACharacterController::SpawnCharacter()
{
	UMyGameInstance* GI = GetGameInstance<UMyGameInstance>();
	if (!IsValid(GI))
		return;
	if (GI->GetIsNewGame()) // 만약 새 게임이면 리턴 <- 이미 기본 캐릭터는 설정 되어 있음
		return;

	FSaveGameData SD = GI->GetSaveGameData(); // 저장된 데이터 로드

	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
		CS->SetSaveGameData(SD); // 캐릭터 스테이트에 저장

	if (!IsValid(mCharacterClassInfoTable))
		return;

	FCharacterClassInfo* CCI = mCharacterClassInfoTable->FindRow<FCharacterClassInfo>(SD.CharacterName, TEXT(""));
	if (!CCI)
		return;

	mCurrentCharacter = GetWorld()->SpawnActor<ACharacterBase>(CCI->CharacterClass, FVector(0, 0, -10000), FRotator::ZeroRotator);
	if (!IsValid(mCurrentCharacter))
		return;

	GetPawn()->Destroy(); // 현재 폰(기본 캐릭터) 파괴 -> 자동으로 UnPossess

	mCharacterIndex = CCI->CharacterIndex;

	mCurrentCharacter->SetActorLocation(SD.CharacterLocation); // 위치
	mCurrentCharacter->SetActorRotation(SD.CharacterRotation); // 각도
	mCurrentCharacter->SetCameraRotation(SD.CameraRotation); // 카메라 각도
	mCurrentCharacter->SetDirYawChange(SD.DirYaw); // 바라보고있는 방향

	Possess(mCurrentCharacter); // 세팅이 끝났으니, 빙의
	
}

void ACharacterController::CharacterChange(FVector Postion, FRotator CameraRotation, float DirYaw)
{
	mCharacterIndex += 1;
	mCharacterIndex %= mCharacterClassInfoArray.Num();
	FCharacterClassInfo* CCI = mCharacterClassInfoArray[mCharacterIndex];
	if (!CCI || !IsValid(CCI->CharacterClass))
		return;

	mCurrentCharacter = GetWorld()->SpawnActor<ACharacterBase>(CCI->CharacterClass, FVector(0, 0, -10000), FRotator::ZeroRotator);

	if (!IsValid(mCurrentCharacter))
		return;

	UnPossess(); // 빙의 해제

	mCurrentCharacter->SetActorLocation(Postion);
	mCurrentCharacter->SetCameraRotation(CameraRotation);
	mCurrentCharacter->SetDirYawChange(DirYaw);
	Possess(mCurrentCharacter);

	if (mCounterEnable) // 패링 지원 가능
	{
		if (IsValid(mCounterCamera)) // 패링 지원 용 카메라
		{
			mCounterCamera->SetActorLocation(mCurrentCharacter->GetActorLocation()); // 위치 조절
			int32 i = FMath::RandRange(0, 1); // 좌우 랜덤 선택
			mCounterCamera->SetActorRotation(FRotator(2.5, DirYaw + 45 * FMath::Pow(-1.0, i), 0)); // 카메라 각도 설정
			SetViewTargetWithBlend(mCounterCamera, 0.1); // 자연스러운 카메라 변경

			if (IsValid(mCounterCameraShake)) // 패링 지원 카메라 쉐이크
				UGameplayStatics::PlayWorldCameraShake(GetWorld(), mCounterCameraShake, mCurrentCharacter->GetActorLocation(), 0, 10000);

			mCounterCamera->PostOn(true); // 포스트 프로세스 설정
		}

		mCurrentCharacter->CounterChange(); // 캐릭터 패링 지원

		if (IsValid(mCounterAttackEffect)) // 패링 지원 이펙트
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), mCounterAttackEffect, mCurrentCharacter->GetActorLocation());

		if (IsValid(mCounterSound)) // 패링 지원 사운드
			UGameplayStatics::PlaySound2D(GetWorld(), mCounterSound);
	}
}
