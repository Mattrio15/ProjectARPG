// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "../InputData/InputData.h"
#include "CharacterController.h"
#include "ItemComponent.h"


ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	mArm = MyObject(USpringArmComponent, "Arm");
	mCamera = MyObject(UCameraComponent, "Camera");
	mDirScene = MyObject(USceneComponent, "DirScene");

	mArm->SetupAttachment(RootComponent);
	mCamera->SetupAttachment(mArm);
	mDirScene->SetupAttachment(RootComponent);

	mArm->SetRelativeLocation(FVector(0, 0, 50));
	mArm->TargetArmLength = 450;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	mNC_DodgeAffterImage = MyObject(UNiagaraComponent, "DodgeAfterImage");
	mNC_DodgeAffterImage->SetupAttachment(GetMesh());
	mNC_DodgeAffterImage->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ObjectFinder(UNiagaraSystem, NS, "/Script/Niagara.NiagaraSystem'/Game/Niagara/NS_DodgeAfterImage.NS_DodgeAfterImage'");
	if (NS.Succeeded())
		mNC_DodgeAffterImage->SetAsset(NS.Object);
	mNC_DodgeAffterImage->bAutoActivate = false;

	mDodgePostProcess = MyObject(UPostProcessComponent, "DodgePostProcess");
	mDodgePostProcess->SetupAttachment(RootComponent);

}

ETeamAttitude::Type ACharacterBase::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other);

	if (!OtherTeamAgent)
		return ETeamAttitude::Neutral;
	if (OtherTeamAgent->GetGenericTeamId().GetId() == 255)
		return ETeamAttitude::Neutral;
	return OtherTeamAgent->GetGenericTeamId() == mTeamId ? ETeamAttitude::Friendly : ETeamAttitude::Hostile;
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
		return CS->GetAbilitySystemComponent();

	return nullptr;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = GetController<APlayerController>())
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* EILPS = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (const UInputData* ID = GetDefault<UInputData>())
					EILPS->AddMappingContext(ID->mMappingContext, 0);
			}
		}
	}

	mDisappearDir = mDirScene->GetForwardVector();

	mAnimInstance = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());

}

void ACharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mDisappear) // 캐릭터가 사라짐 <- 변경 전 캐릭터에 적용
		SetActorLocation(GetActorLocation() - mDisappearDir * 5); // 현재 위치에서 뒤로감
	
	if (mForward) // 앞으로 전진 <- 변경 후 캐릭터에 적용
	{
		mForwardTimer += DeltaTime; // 타이머
		if (mForwardTimer < 0.5) // 0.5초 동안만 작동
			AddMovementInput(mDisappearDir, 1); // 앞으로 나아감
		else // 0.5초 후
		{
			mForward = false; // 앞으로 나아가기 금지
			mForwardTimer = 0; // 타이머는 0초로
		}
	}

	if (!mPDEnable) // 완벽한 회피가 불가능할 때
	{
		for (int32 i = 0; i < mTA_Target.Num(); ++i) // 타깃을 순회
		{
			if (!IsValid(mTA_Target[i])) // 타깃이 유효하지 않음
			{
				mTA_Target.RemoveAtSwap(i); // 배열에서 타깃을 지움
				i = -1; // 지웠으니 다시 확인
				continue;
			}
			if (!mTarget.IsValid()) // 주 타깃이 유효하지 않다면 <- TWeakObjectPtr을 사용했으므로 .IsValid()
			{
				mTarget = mTA_Target[i]; // 타깃 재설정
				continue;
			}

			// 타깃과의 거리가 다른 타깃(몬스터들)보다 거리가 멀다면, 주 타깃 갱신
			if ((mTarget->GetActorLocation() - GetActorLocation()).Length() > (mTA_Target[i]->GetActorLocation() - GetActorLocation()).Length())
				mTarget = mTA_Target[i];
		}
	}

	if (mTarget.IsValid()) // 타깃이 유효하다면
	{
		// 나아갈 방향 설정
		mDirVector = mTarget->GetActorLocation() - GetActorLocation() - FVector(0, 0, (mTarget->GetActorLocation() - GetActorLocation()).Z);
		mDirVector.Normalize();
	}

}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	const UInputData* ID = GetDefault<UInputData>();
	if (!IsValid(ID))
		return;

	Input->BindAction(ID->mMove, ETriggerEvent::Started, this, &ACharacterBase::CharacterMoveStart);
	Input->BindAction(ID->mMove, ETriggerEvent::Triggered, this, &ACharacterBase::CharacterMove);
	Input->BindAction(ID->mMove, ETriggerEvent::Completed, this, &ACharacterBase::CharacterMoveOff);
	Input->BindAction(ID->mCameraRotation, ETriggerEvent::Triggered, this, &ACharacterBase::CharacterCameraRotation);
	Input->BindAction(ID->mCharacterChange, ETriggerEvent::Started, this, &ACharacterBase::CharacterChange);
	Input->BindAction(ID->mDodge, ETriggerEvent::Started, this, &ACharacterBase::CharacterDodge);
	Input->BindAction(ID->mAttack, ETriggerEvent::Started, this, &ACharacterBase::CharacterAttack);
	Input->BindAction(ID->mAttack, ETriggerEvent::Triggered, this, &ACharacterBase::CharacterAttackTriggered);
	Input->BindAction(ID->mAttack, ETriggerEvent::Completed, this, &ACharacterBase::CharacterAttackCompleted);
	Input->BindAction(ID->mSkill, ETriggerEvent::Started, this, &ACharacterBase::CharacterSkill);
	Input->BindAction(ID->mSkill, ETriggerEvent::Triggered, this, &ACharacterBase::CharacterSkillTriggered);
	Input->BindAction(ID->mSkill, ETriggerEvent::Completed, this, &ACharacterBase::CharacterSkillCompleted);
	Input->BindAction(ID->mUltimate, ETriggerEvent::Started, this, &ACharacterBase::CharacterUltimate);
	Input->BindAction(ID->mInventory, ETriggerEvent::Started, this, &ACharacterBase::CharacterInventory);
	Input->BindAction(ID->mQuickSlot_1, ETriggerEvent::Started, this, &ACharacterBase::CharacterQuickSlot_1);
	Input->BindAction(ID->mQuickSlot_2, ETriggerEvent::Started, this, &ACharacterBase::CharacterQuickSlot_2);
	Input->BindAction(ID->mQuickSlot_3, ETriggerEvent::Started, this, &ACharacterBase::CharacterQuickSlot_3);
	Input->BindAction(ID->mPause, ETriggerEvent::Started, this, &ACharacterBase::CharacterPause);
	Input->BindAction(ID->mFKey, ETriggerEvent::Started, this, &ACharacterBase::CharacterFKey);

}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
	{
		CS->SetCharacterName(mName); // 캐릭터 스테이트에 이름 넘겨주기
		CS->InitAbilitySystemComponent(this); // 어빌리티 시스템 초기화
	}
}

void ACharacterBase::CharacterMoveStart(const FInputActionInstance& Instance)
{
	if (mShowUI) // UI가 보이고 있다면
	{
		mShowUI = !mShowUI; // 안보인다고 알려주기
		ShowUI(mShowUI); // UI 숨기기
	}

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
	{
		AIB->SetMovingDodge(true); // 이동하면서 회피 가능
		AIB->SetStopMoveDodge(true); // 이동하면서 회피 가능 <- 뮤리엘 전용
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (IsValid(ASC))
	{
		// GT 추가 <- 이동 중
		if(!ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Moving"))))
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Moving")));
	}
}

void ACharacterBase::CharacterMove(const FInputActionInstance& Instance)
{
	FVector2D Dir = Instance.GetValue().Get<FVector2D>();
	mDirVector = mDirScene->GetForwardVector() * Dir.X + mDirScene->GetRightVector() * Dir.Y;
	mDirVector.Normalize();
	mDodgeDirVector = mDirScene->GetForwardVector() * Dir.X + mDirScene->GetRightVector() * Dir.Y;
	mDodgeDirVector.Normalize();
	mMoving = true;

	if (!mMoveEnable)
		return;

	AddMovementInput(mDirScene->GetForwardVector(), Dir.X);
	AddMovementInput(mDirScene->GetRightVector(), Dir.Y);

	FVector V = GetCharacterMovement()->Velocity;
	V.Normalize();
	mDisappearDir = V;

}

void ACharacterBase::CharacterMoveOff(const FInputActionInstance& Instance)
{
	mDirVector = FVector(0, 0, 0);
	mMoving = false;

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
	{
		AIB->SetMovingDodge(false);
		AIB->SetStopMoveDodge(false);
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (IsValid(ASC))
	{
		if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Moving"))))
			ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Moving")));
	}
}

void ACharacterBase::CharacterCameraRotation(const FInputActionInstance& Instance)
{
	if (mShowUI)
		return;

	if (!mCameraRotationEnable)
		return;

	FVector Dir = Instance.GetValue().Get<FVector>();

	FRotator Rot = mArm->GetRelativeRotation() + FRotator(Dir.Y, Dir.X, 0);

	if (Rot.Pitch > 85)
		Rot.Pitch = 85;
	if (Rot.Pitch < -85)
		Rot.Pitch = -85;

	mArm->SetRelativeRotation(Rot);
	mDirScene->SetRelativeRotation(FRotator(0, Rot.Yaw, 0));
}

void ACharacterBase::CharacterChange(const FInputActionInstance& Instance)
{
	if (mShowUI) // UI가 보일 때에는 변경 금지
		return;

	if (ACharacterController* CC = GetController<ACharacterController>())
	{
		if (UAnimInstanceBase* AB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance()))
		{
			// 변경 전 캐릭터 삭제
			GetWorld()->GetTimerManager().SetTimer(mChangeTimer, this, &ACharacterBase::CharacterDisappear, 0.5, false);

			GetCapsuleComponent()->SetCollisionProfileName(TEXT("Spectator")); // 진로에 방해되지 않게 콜리젼 변경
						
			FVector Pos = GetActorLocation(); // 변경 전 캐릭터 위치 저장

			ACharacterState* CS = GetPlayerState<ACharacterState>();
			if (IsValid(CS))
			{
				CS->SaveCharacterInfo(); // 캐릭터의 정보 저장
				CS->PlayButtonAnimation(3); // 캐릭터 변경 아이콘 이펙트 실행
			}

			// GT로 완벽한 회피 가능 여부 확인
			mPDEnable = GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.PDEnable")));
			if (mPDEnable) // 완벽한 회피 가능
			{
				CC->SetCounterEnable(true); // 패링 지원 가능
				FRotator Rot = (mTarget->GetActorLocation() - GetActorLocation()).Rotation(); // 대상 각도 설정
				CC->CharacterChange(Pos, mArm->GetRelativeRotation(), Rot.Yaw); // 위치, 카메라 각도, 대상 각도 전달
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.5); // 슬로우모션 시작

				Destroy(); // 액터 파괴
			}
			else
			{
				CC->CharacterChange(Pos, mArm->GetRelativeRotation(), AB->GetDirYaw()); // 위치, 카메라 각도, 바라보고있는 방향 전달
				DisappearAnimation(); // 캐릭터 삭제
			}

			mDisappear = true;
			GetMesh()->SetRelativeRotation(FRotator(0, AB->GetDirYaw() - 90, 0));
			
		}
	}
}

void ACharacterBase::CharacterDodge(const FInputActionInstance& Instance)
{
	if (mShowUI) // UI가 있을 시 사용 금지
		return;

	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
		CS->PlayButtonAnimation(1); // 회피 버튼 클릭 이펙트 재생

	if (GetPlayerState<ACharacterState>()->PlayGA_Dodge()) // GA 실행
	{
		mAttackEnable = false; // 회피 중 공격 불가
		mSkillEnable = false; // 회피 중 스킬 불가
	}
}

void ACharacterBase::CharacterAttack(const FInputActionInstance& Instance)
{
	if (mShowUI)
		return;

	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
		CS->PlayButtonAnimation(0);

	if (!mAttackEnable)
		return;

	ShortAttack();
	MoveAttack();

}

void ACharacterBase::CharacterAttackTriggered(const FInputActionInstance& Instance)
{
	if (mShowUI)
		return;

	if (!mAttackEnable)
		return;

	LongAttack();
}

void ACharacterBase::CharacterAttackCompleted(const FInputActionInstance& Instance)
{
}

void ACharacterBase::CharacterSkill(const FInputActionInstance& Instance)
{
	if (mShowUI)
		return;

	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
		CS->PlayButtonAnimation(2);

	if (!mSkillEnable)
		return;

	mDodgeEnable = false;
	ShortSkill();
	LongSkillReady();
}

void ACharacterBase::CharacterSkillTriggered(const FInputActionInstance& Instance)
{
	if (mShowUI)
		return;

}

void ACharacterBase::CharacterSkillCompleted(const FInputActionInstance& Instance)
{
	if (mShowUI)
		return;

	LongSkillCompleted();
}

void ACharacterBase::CharacterUltimate(const FInputActionInstance& Instance)
{
	if (mShowUI)
		return;

	if (!mUltimateEnable)
		return;

	Ultimate();
	mUltimateEnable = false;

	mDodgeEnable = false;
}

void ACharacterBase::CharacterInventory(const FInputActionInstance& Instance)
{
	mShowUI = !mShowUI;
	ShowUI(mShowUI);
	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
		CS->ShowInventory(mShowUI);
}

void ACharacterBase::CharacterQuickSlot_1(const FInputActionInstance& Instance)
{
	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
	{
		UItemComponent* IC = CS->GetItemComponent();
		if (IsValid(IC))
			IC->UseQuickSlotItem(0);

		CS->PlayQuickSlotAnimation(0);
	}
}

void ACharacterBase::CharacterQuickSlot_2(const FInputActionInstance& Instance)
{
	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
	{
		UItemComponent* IC = CS->GetItemComponent();
		if (IsValid(IC))
			IC->UseQuickSlotItem(1);

		CS->PlayQuickSlotAnimation(1);
	}
}

void ACharacterBase::CharacterQuickSlot_3(const FInputActionInstance& Instance)
{
	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
	{
		UItemComponent* IC = CS->GetItemComponent();
		if (IsValid(IC))
			IC->UseQuickSlotItem(2);

		CS->PlayQuickSlotAnimation(2);
	}
}

void ACharacterBase::CharacterPause(const FInputActionInstance& Instance)
{
	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
		CS->ShowPause();
}

void ACharacterBase::CharacterFKey(const FInputActionInstance& Instance)
{
	if (mNPCTalking)
	{
		ACharacterState* CS = GetPlayerState<ACharacterState>();
		if (IsValid(CS))
			CS->PlayMiniGame();
	}

}

void ACharacterBase::CharacterDisappear()
{
	Destroy();
}

void ACharacterBase::DisappearAnimation()
{
	UAnimationAsset* AA = mDisappearAnim.LoadSynchronous();
	if (IsValid(AA))
		GetMesh()->PlayAnimation(AA, true);
}

void ACharacterBase::ShortAttack()
{
	if (IsValid(mAnimInstance))
		mAnimInstance->PlayShortAttack();
}

void ACharacterBase::DodgeAttack()
{
}

void ACharacterBase::SetCameraRotation(FRotator CameraRotation)
{
	mDirScene->SetRelativeRotation(FRotator(0,CameraRotation.Yaw,0));
	mArm->SetRelativeRotation(CameraRotation);
}

void ACharacterBase::SetDirYawChange(float DirYaw)
{
	if (UAnimInstanceBase* AB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance()))
	{
		float RYaw = FMath::RandRange(-45, 45); // -45도부터 45도까지 랜덤하게 설정
		AB->SetDirYaw(DirYaw); // 캐릭터가 바라보는 방향은 애님 인스턴스 블루프린트의 Rotate Root Bone 노드 사용
		mDisappearDir = FRotator(0, DirYaw + RYaw, 0).Vector(); // 캐릭터가 사라질 방향 설정
		mForward = true; // 앞으로 나아가기
	}
}

void ACharacterBase::SetTargeting(AActor* Target)
{
	if (!mTA_Target.Contains(Target))
		mTA_Target.Add(Target);
}

void ACharacterBase::ShowUI(bool A)
{
	ACharacterController* AC = GetController<ACharacterController>();
	ACharacterState* CS = GetPlayerState<ACharacterState>();

	if (!IsValid(AC) || !IsValid(CS))
		return;

	if (A)
	{
		FInputModeGameAndUI Mode;
		AC->SetInputMode(Mode);
		AC->bShowMouseCursor = true;
	}
	else
	{
		FInputModeGameOnly Mode;
		AC->SetInputMode(Mode);
		AC->bShowMouseCursor = false;
		CS->ShowUI(A);
	}
}

void ACharacterBase::SetNPCTalking(bool A)
{
	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (!IsValid(CS))
		return;

	mNPCTalking = A;

	CS->ShowFKey(mNPCTalking);
}

void ACharacterBase::CounterChange()
{
	ACharacterController* CC = GetController<ACharacterController>();
	if (!IsValid(CC))
		return;

	if (!CC->GetCounterEnable()) // 기본은 패링 지원 불가능 상태
		return;

	mMoveEnable = false;
	CC->SetCounterEnable(false); // 이 함수가 실행 됐다는 것은, 패링 지원 가능했다는 것 -> 다시 불가능 상태로
	mForward = false; // 패링 지원 시에는 앞으로 나아갈 필요가 없음
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayDodgeAndCounterAttack(1); // 회피 공격 애니메이션은 0(사용하지는 않음), 패링 지원 애니메이션은 1

	// UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Niagara/NS_Wave.NS_Wave'"));
	// if (IsValid(NS))
	// 	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS, GetActorLocation()); // 일렁이는 나이아가라 효과
	
	TArray<FHitResult> Results;
	bool Collision = GetWorld()->SweepMultiByProfile(Results, GetActorLocation(), GetActorLocation(), FQuat::Identity, TEXT("PlayerSkill"),
		FCollisionShape::MakeSphere(350)); // 패링 지원 공격의 범위
	if (Collision)
	{
		ACharacterState* CS = GetPlayerState<ACharacterState>();
		for (int32 i = 0; i < Results.Num(); ++i) // 패링 지원 공격을 받은 대상들을 순회
		{
			AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
			if (IsValid(CS) && IsValid(Monster)) // 몬스터라면 아리 실행
			{
				Monster->PlayKnockback(GetActorLocation()); // 몬스터는 현재 위치로부터 넉백
				CS->PlayGE_CounterAttack(mName, Monster->GetAbilitySystemComponent()); // 캐릭터 스테이트에서 Gameplay Effect 실행
			}
		}
	}
}

void ACharacterBase::GiveDamage(AActor* Target, float Damage, bool Critical)
{
	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
	{
		AMonsterBase* MB = Cast<AMonsterBase>(Target);
		if (IsValid(MB))
		{
			MB->SetIsHit(true);
		}
	}
}

void ACharacterBase::LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	Super::LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
	
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
	{
		AIB->SetIsKnockback(true);
		AIB->StopAllMontages(0.1);
	}
}
