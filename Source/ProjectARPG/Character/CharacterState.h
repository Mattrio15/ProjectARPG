// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"
#include "CharacterInfo.h"
#include "GameFramework/PlayerState.h"
#include "CharacterState.generated.h"

struct FSaveGameData;
class UItemComponent;

class UCharacterAttributeSet;
class UAttributeDataAsset;
class UAbilityDataAsset;
class UDA_CharacterGE;

class UCharacterHPWidget;
class UCharacterInventory;
class UCharacterInfoWidget;
class UMiniGameWidget;

USTRUCT(BlueprintType)
struct FCharacterGE : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> GE_Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> GE_CounterAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> GE_Skill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag GT_Elemental;

};

UCLASS()
class PROJECTARPG_API ACharacterState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ACharacterState();

protected:
	FName mCharacterName; // 캐릭터 이름
	TMap<FName, FCharacterInfo> mTM_CharacterInfo; // 이름에 따른 캐릭터 정보

	UPROPERTY()
	TObjectPtr<UDataTable> mDT_CharacterInfo; // 캐릭터 정보 데이터 테이블

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> mASC; // 어빌리티 시스템 컴포넌트
	UPROPERTY()
	TObjectPtr<UCharacterAttributeSet> mAS_Character; // 캐릭터 어트리뷰트 셋

	UPROPERTY()
	TObjectPtr<UAttributeDataAsset> mDA_AttributeTag; // 어트리뷰트 데이터 애셋

	UPROPERTY()
	TObjectPtr<UDA_CharacterGE> mDA_CharacterGE; // 캐릭터가 사용할 GE 데이터 애셋
	UPROPERTY()
	TObjectPtr<UDataTable> mDT_CharacterGE; // 캐릭터가 사용할 GE 데이터 테이블
	UPROPERTY()
	TMap<FName, FCharacterGE> mTM_CharacterGE; // 캐릭터가 사용할 GE를 이름으로 찾을 맵
	UPROPERTY()
	TObjectPtr<UAbilityDataAsset> mDA_CharacterGA; // 캐릭터가 사용할 어빌리티 데이터 애셋

	UPROPERTY()
	TSubclassOf<UUserWidget> mMainWidgetClass; // 메인 위젯 클래스
	UPROPERTY()
	TObjectPtr<UCharacterHPWidget> mHPWidget; // HP 위젯
	UPROPERTY()
	TObjectPtr<UCharacterInventory> mInventoryWidget; // 인벤토리 위젯
	UPROPERTY()
	TObjectPtr<UCharacterInfoWidget> mStatusWidget;
	UPROPERTY()
	TObjectPtr<UUserWidget> mMainWidget; // 메인 위젯
	UPROPERTY()
	TObjectPtr<UUserWidget> mPauseWidget; // 일시정지 위젯

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UItemComponent> mItemComponent; // 아이템 컴포넌트
	
	UPROPERTY()
	TSubclassOf<UGameplayAbility> mGA_DodgeClass; // 회피 GA

	UPROPERTY()
	TSubclassOf<UGameplayEffect> mGE_Init; // 초기화 GE

	UPROPERTY()
	TSubclassOf<UGameplayEffect> mGE_HPRegenClass; // 체력 회복 GE

	UPROPERTY()
	TSubclassOf<UGameplayEffect> mGE_ManaClass; // 공격시 마나 회복 GE

	UPROPERTY()
	TObjectPtr<UMiniGameWidget> mMiniGameWidget; // 미니게임 위젯

	UPROPERTY()
	TObjectPtr<UAudioComponent> mMainLevelBGM;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const { return mASC; }

protected:
	virtual void BeginPlay();

	void InitWidget(); // 위젯 초기화 함수
	void InitCharacterGE(); // 캐릭터 GE 초기화 함수

public:
	void InitAbilitySystemComponent(AActor* Avatar); // ASC 초기화

protected:
	void SetTMCharacterGE(); // 캐릭터 GE 맵 설정 함수
	void SetCharacterMoveSpeed(); // 캐릭터 이동속도 설정 함수
	void ApplyGE_Init(); // 초기화 GE 적용 함수
	void ApplyCharacterGA(); // 캐릭터 GA 적용 함수

public:
	void SaveCharacterInfo(); // 캐릭터 정보 저장

protected:
	FGameplayEffectSpecHandle GetGameplayEffectSpecHandle(TSubclassOf<UGameplayEffect> GEClass);
	void PlayGameplayEffect(TSubclassOf<UGameplayEffect> GEClass, FGameplayTag GT_Elemental, UAbilitySystemComponent* ASC);

public:
	void PlayGE_Attack(FName Name, UAbilitySystemComponent* ASC); // 공격 용 GE 실행 함수
	void PlayGE_CounterAttack(FName Name, UAbilitySystemComponent* ASC); // 패링 지원 용 GE 실행 함수
	void PlayGE_Skill(FName Name, UAbilitySystemComponent* ASC); // 스킬 용 GE 실행 함수

	bool PlayGA_Dodge(); // 회피 용 GA 실행 함수


public:
	UFUNCTION(BlueprintCallable)
	void ShowMainWidget(bool A); // 메인 위젯 보이기 여부
	void ShowUI(bool A); // UI 보이기 여부
	void ShowInventory(bool A); // 인벤토리 보이기 여부
	void ShowPause(); // 일시정지 위젯 보이기 여부
	void ShowFKey(bool A); // F키 위젯 보이기 여부
	void ShowStatus(bool A); // 스탯 창 보이기 여부

	void PlayButtonAnimation(int32 Index); // UI에서 버튼 이펙트 실행 함수
	void PlayQuickSlotAnimation(int32 Index); // UI에서 퀵슬롯 애니메이션 실행 함수

	void PlayMiniGame(); // 미니게임 실행 함수

public:
	UCharacterHPWidget* GetHPWidget() { return mHPWidget; }
	FName GetCharacterName() { return mCharacterName; }
	FCharacterInfo GetCharacterInfo() { return mTM_CharacterInfo[mCharacterName]; }
	void SetCharacterName(FName Name) { mCharacterName = Name; }

	UItemComponent* GetItemComponent() { return mItemComponent; }

	UFUNCTION(BlueprintCallable)
	void GetItem(UItemDataAsset* Item); // 아이템 획득 함수

	void SetCharacterFace(); // 캐릭터 초상화 설정 함수

public:
	FSaveGameData GetSaveGameData();

	void SetSaveGameData(FSaveGameData Data);


};
