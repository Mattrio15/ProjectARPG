// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "CounterCamera.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

class ACharacterBase;

USTRUCT(BlueprintType)
struct FCharacterClassInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACharacterBase> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CharacterIndex;

};

UCLASS()
class PROJECTARPG_API ACharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	ACharacterController();

protected:
	int32 mCharacterIndex = 0; // 캐릭터 번호 <- 각 캐릭터의 번호가 아니라, 단순히 캐릭터 순서 용
	bool mCounterEnable = false; // 패링 지원 가능 여부

	UPROPERTY()
	TObjectPtr<ACounterCamera> mCounterCamera; // 패링 지원 용 카메라

	FTimerHandle mSpawnTimer; // 캐릭터 스폰 타이머

	UPROPERTY()
	TObjectPtr<ACharacterBase> mCurrentCharacter; // 현재 사용하고 있는 캐릭터

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> mCounterAttackEffect; // 캐릭터 베이스에서 넘어옴 / 패링 지원 용 나이아가라 이펙트
	
	UPROPERTY()
	TObjectPtr<USoundBase> mCounterSound; // 패링 지원 용 사운드

	UPROPERTY()
	TSubclassOf<UCameraShakeBase> mCounterCameraShake; // 패링 지원 용 카메라 쉐이크

	UPROPERTY()
	UDataTable* mCharacterClassInfoTable;

	TArray<FCharacterClassInfo*> mCharacterClassInfoArray;

protected:
	virtual void BeginPlay() override;

protected:
	void SpawnCharacter(); // 캐릭터 스폰

public:
	void CharacterChange(FVector Postion, FRotator CameraRotation, float DirYaw); // 캐릭터 변경 함수
	void SetCounterEnable(bool A) { mCounterEnable = A; } // 패링 지원 가능 설정

	bool GetCounterEnable() { return  mCounterEnable; } // 패링 지원 가능 여부 반환

};
