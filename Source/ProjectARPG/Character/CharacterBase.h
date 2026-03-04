// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Game_Info.h"					// 베이스 헤더
#include "InputAction.h"					// 입력
#include "AnimInstanceBase.h"				// 에님 인스턴스
#include "CharacterState.h"					// 캐릭터 스테이트
#include "../AI/Monster/MonsterBase.h"		// 몬스터
#include "GenericTeamAgentInterface.h"		// 팀
#include "GameFramework/Character.h"		// 캐릭터 프레임워크
#include "CharacterBase.generated.h"		// GENERATED_BODY

UCLASS()
class PROJECTARPG_API ACharacterBase : public ACharacter,public IGenericTeamAgentInterface // public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();

protected:
	FGenericTeamId mTeamId = FGenericTeamId(1);				  // 팀 아이디
															  
	UPROPERTY(VisibleAnywhere)								  // 어디서나 보이게
	TObjectPtr<USpringArmComponent> mArm;					  // 스프링 암
	UPROPERTY(VisibleAnywhere)								  
	TObjectPtr<UCameraComponent> mCamera;					  // 카메라
	UPROPERTY()												  
	TObjectPtr<USceneComponent> mDirScene;					  // 회전 용 신 컴포넌트
															  
	bool mMoveEnable = true;								  // 움직임 여부
	bool mCameraRotationEnable = true;						  // 카메라 회전 여부
															  
	FTimerHandle mChangeTimer;								  // 캐릭터 변경 타이머
	bool mDisappear = false;								  // 캐릭터 사라짐 여부
	FVector mDisappearDir;									  // 캐릭터 사라지는 방향
															  
	bool mForward = false;									  // 캐릭터 전진 유무
	float mForwardTimer = 0;								  // 캐릭터 전진 타이머
															  
	bool mDodge = false;									  // 회피 중 여부
	float mDodgeTimer = 0;									  // 회피 타이머
	FVector mDodgeDir;										  // 회피 방향
	bool mDodgeEnable = true;								  // 회피 가능 여부
															  
	FVector mDirVector = FVector(0, 0, 0);					  // 바라보는 방향
	bool mMoving = false;									  // 움직임 여부
	FVector mDodgeDirVector = FVector(0, 0, 0);				  // 이동 중 회피 방향
															  
	bool mPDEnable = false;									  // 완벽한 회피 가능 여부
	FTimerHandle mDilationTimer;							  // 완벽한 회피 시 슬로우모션 타이머
	FTimerHandle mPDFail;									  // 완벽한 회피 실패 시 타이머
															  
	UPROPERTY()												  
	TObjectPtr<UNiagaraComponent> mDodgeAfterImage;			  // 완벽한 회피용 나이아가라
	UPROPERTY(VisibleAnywhere)								  
	TObjectPtr<UPostProcessComponent> mDodgePostProcess;	  // 완벽한 회피용 포스트 프로세스
	bool mDPPEnable = false;								  // 완벽한 회피용 포스트 프로세스 사용 가능 여부
															  
	bool mAttackEnable = true;								  // 공격 가능 여부
															  
	bool mSkillEnable = true;								  // 스킬 가능 여부
															  
	bool mUltimateDilation = false;							  // 궁극기 슬로우모션 여부
	bool mUltimateEnable = true;							  // 궁극기 사용 가능 여부
															  
	TArray<AActor*> mTargetArray;							  // 타깃(몬스터) 배열
	UPROPERTY()												  
	TWeakObjectPtr<AActor> mTarget;							  // 타깃(몬스터)
															  
	FName mName;											  // 캐릭터 이름
															  
	bool mShowUI = false;									  // UI 보이기 여부
	bool mNPCTalking = false;								  // NPC 대화 여부

public:
	// 팀 아이디 관련 함수들
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) {}				
	virtual FGenericTeamId GetGenericTeamId() const { return mTeamId; }			
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;			

protected:
	// 기본 함수들
	virtual void BeginPlay() override;														 
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);							 

public:
	virtual void Tick(float DeltaTime) override;											 

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;	
	virtual void PossessedBy(AController* NewController) override;							

protected:
	// 입력 관련
	void CharacterMoveStart(const FInputActionInstance& Instance);		  // 이동 시작
	void CharacterMove(const FInputActionInstance& Instance);			  // 이동 중
	void CharacterMoveOff(const FInputActionInstance& Instance);		  // 이동 종료
	void CharacterCameraRotation(const FInputActionInstance& Instance);	  // 카메라 회전 시작
	void CharacterChange(const FInputActionInstance& Instance);			  // 캐릭터 변경
	void CharacterDodge(const FInputActionInstance& Instance);			  // 회피
	void CharacterAttack(const FInputActionInstance& Instance);			  // 공격 시작
	void CharacterAttackTriggered(const FInputActionInstance& Instance);  // 공격 중
	void CharacterAttackCompleted(const FInputActionInstance& Instance);  // 공격 완료
	void CharacterSkill(const FInputActionInstance& Instance);			  // 스킬 시작
	void CharacterSkillTriggered(const FInputActionInstance& Instance);	  // 스킬 중
	void CharacterSkillCompleted(const FInputActionInstance& Instance);	  // 스킬 완료
	void CharacterUltimate(const FInputActionInstance& Instance);		  // 궁극기
	void CharacterInventory(const FInputActionInstance& Instance);		  // 인벤토리
	void CharacterQuickSlot_1(const FInputActionInstance& Instance);	  // 퀵슬롯1
	void CharacterQuickSlot_2(const FInputActionInstance& Instance);	  // 퀵슬롯2
	void CharacterQuickSlot_3(const FInputActionInstance& Instance);	  // 퀵슬롯3
	void CharacterPause(const FInputActionInstance& Instance);			  // 일시 정지
	void CharacterFKey(const FInputActionInstance& Instance);			  // F키

protected:
	void CharacterDisappear();											  // 캐릭터 사라지기
	virtual void DisappearAnimation() {}								  // 캐릭터 사라지기 애니메이션
	void PerfectDodgeFail() { mPDEnable = false; }						  // 완벽한 회피 실패

	virtual void ShortAttack(){}										  // 짧게 누르는 공격
	virtual void LongAttack(){}											  // 길게 누르는 공격
	virtual void MoveAttack(){}											  // 이동 가능한 공격
	virtual void ShortSkill(){}											  // 짧게 누르는 스킬
	virtual void LongSkillReady(){}										  // 길게 누르는 스킬 준비
	virtual void LongSkill(){}											  // 길게 누르는 스킬
	virtual void LongSkillCompleted(){}									  // 길게 누르는 스킬 완료
	virtual void Ultimate(){}											  // 궁극기
	virtual void DodgeAttack();											  // 회피 공격			<- 사용하지는 않음

public:
	virtual void ShortAttackEffect(int32 Index, float Yaw){}			  // 짧은 공격 이펙트
	virtual void ShortSkillEffect(int32 Index, float Yaw) {}			  // 짧은 스킬 이펙트
	virtual void LongSkillEffect(int32 Index, float Yaw){}				  // 긴 스킬 이펙트
	virtual void UltimateEffect(){}										  // 궁극기 이펙트
	virtual void RealAttack(int32 Index){}								  // 실제 공격
	virtual void RealSkill(){}											  // 실제 스킬
	virtual void RealDodgeAttack(){}									  // 실제 회피 공격		<- 사용하지는 않음
	virtual void RealUltimate(){}										  // 실제 궁극기

public:
	void SetCameraRotation(FRotator CameraRotation);					  // 카메라 회전 각도 설정
	void SetDirYawChange(float DirYaw);									  // 캐릭터 변경 시 바라보는 방향 설정
	void SetMoveEnable(bool A) { mMoveEnable = A; }						  // 움직임 가능 여부 설정
	void SetDodgeEnable(bool A) { mDodgeEnable = A; }					  // 회피 가능 여부 설정
	void SetAttackEnable(bool A) { mAttackEnable = A; }					  // 공격 가능 여부 설정
	void SetSkillEnable(bool A) { mSkillEnable = A; }					  // 스킬 가능 여부 설정
	void SetTargeting(AActor* Target);									  // 타깃(몬스터) 설정

	void ShowUI(bool A);												  // UI 보이기 여부 설정
	void SetNPCTalking(bool A);											  // NPC 대화 중 여부 설정

	FVector GetDirVector() { return mDirVector; }						  // 바라보는 방향 가져오기
	float GetDirYaw() { return mDirScene->GetRelativeRotation().Yaw; }	  // 바라보는 방향의 Yaw값 가져오기
	FName GetCharacterName() { return mName; }							  // 캐릭터 이름 가져오기
	UPostProcessComponent* GetDPP() { return mDodgePostProcess; }		  // 완벽한 회피 용 포스트 프로세스 가져오기
	UNiagaraComponent* GetDAI() { return mDodgeAfterImage; }			  // 완벽한 회피 용 나이아가라 가져오기
	FRotator GetCameraRotation() { return mArm->GetRelativeRotation(); }  // 카메라 회전 각도 가져오기

public:
	void CounterChange();												  // 반격 회피

	void GiveDamage(AActor* Target, float Damage, bool Critical);		  // 데미지 주기			<- GE로 대체

	virtual void LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride);	// 캐릭터 넉백


protected:
	UFUNCTION()
	void PDEnd() { mPDEnable = false; }									  // 완벽한 회피 종료 함수



};
