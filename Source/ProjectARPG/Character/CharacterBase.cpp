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

	mArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mDirScene = CreateDefaultSubobject<USceneComponent>(TEXT("DirScene"));

	mArm->SetupAttachment(RootComponent);
	mCamera->SetupAttachment(mArm);
	mDirScene->SetupAttachment(RootComponent);

	mArm->SetRelativeLocation(FVector(0, 0, 50));
	mArm->TargetArmLength = 450;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	mDodgeAfterImage = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DodgeAfterImage"));
	mDodgeAfterImage->SetupAttachment(GetMesh());
	mDodgeAfterImage->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		NS(TEXT("/Script/Niagara.NiagaraSystem'/Game/Niagara/NS_DodgeAfterImage.NS_DodgeAfterImage'"));
	if (NS.Succeeded())
		mDodgeAfterImage->SetAsset(NS.Object);
	mDodgeAfterImage->bAutoActivate = false;

	mDodgePostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("DodgePostProcess"));
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

}

void ACharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mDisappear)
		SetActorLocation(GetActorLocation() - mDisappearDir * 2);

	if (mForward)
	{
		mForwardTimer += DeltaTime;
		if (mForwardTimer < 0.3)
			AddMovementInput(mDisappearDir, 1);
		else
		{
			mForward = false;
			mForwardTimer = 0;
		}
	}

	if (mDodge)
	{
		mDodgeTimer += DeltaTime;
		if (mDodgeTimer < 0.3)
		{
			float DodgeLen = 7.5;
			if (mPDEnable)
			{
				if (mDodgePostProcess->Settings.VignetteIntensity < 1)
				{
					mDodgePostProcess->Settings.SceneFringeIntensity += DeltaTime * 50;
					mDodgePostProcess->Settings.VignetteIntensity += DeltaTime * 10;
				}
				DodgeLen *= 2;
			}
			if(mMoving)
				AddMovementInput(mDodgeDir, 1);
			else
				SetActorLocation(GetActorLocation() + mDodgeDir * DodgeLen);
		}
		else
		{
			mMoveEnable = true;
			mDodge = false;
			mDodgeTimer = 0;
			mPDEnable = false;

			mSkillEnable = true;

			UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
			if (IsValid(AIB))
			{
				AIB->SetDodge(false);
				AIB->SetAttackEnable(true);
			}
		}
	}
	if (mDPPEnable)
	{
		if (mDodgePostProcess->Settings.VignetteIntensity > 0)
		{
			mDodgePostProcess->Settings.SceneFringeIntensity -= DeltaTime * 75;
			mDodgePostProcess->Settings.VignetteIntensity -= DeltaTime * 15;
		}
		else
		{
			mDPPEnable = false;
			mDodgePostProcess->Settings.bOverride_SceneFringeIntensity = false;
			mDodgePostProcess->Settings.bOverride_VignetteIntensity = false;
			mDodgePostProcess->Settings.SceneFringeIntensity = 0;
			mDodgePostProcess->Settings.VignetteIntensity = 0;
		}
	}

	if (!mPDEnable)
	{
		for (int32 i = 0; i < mTargetArray.Num(); ++i)
		{
			if (!IsValid(mTargetArray[i]))
			{
				mTargetArray.RemoveAtSwap(i);
				i = -1;
				continue;
			}
			if (!IsValid(mTarget))
			{
				mTarget = mTargetArray[i];
				continue;
			}

			if ((mTarget->GetActorLocation() - GetActorLocation()).Length() > (mTargetArray[i]->GetActorLocation() - GetActorLocation()).Length())
				mTarget = mTargetArray[i];
		}
	}

	if (IsValid(mTarget))
	{
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

}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ACharacterState* CS = GetPlayerState<ACharacterState>();
	if (IsValid(CS))
	{
		CS->SetCharacterName(mName);
		CS->InitAbilitySystemComponent(this);
	}
}

void ACharacterBase::CharacterMoveStart(const FInputActionInstance& Instance)
{
	if (mShowUI)
	{
		mShowUI = !mShowUI;
		ShowUI(mShowUI);
	}

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
	{
		AIB->SetMovingDodge(true);
		AIB->SetStopMoveDodge(true);
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
	if (mShowUI)
		return;

	if (ACharacterController* CC = GetController<ACharacterController>())
	{
		if (UAnimInstanceBase* AB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance()))
		{
			GetWorld()->GetTimerManager().SetTimer(mChangeTimer, this, &ACharacterBase::CharacterDisappear, 0.5, false);

			GetCapsuleComponent()->SetCollisionProfileName(TEXT("Spectator"));
						
			FVector Pos = GetActorLocation();

			ACharacterState* CS = GetPlayerState<ACharacterState>();
			if (IsValid(CS))
				CS->SaveCharacterInfo();

			if (mPDEnable)
			{
				CC->SetCounterEnable(true);
				FRotator Rot = (mTarget->GetActorLocation() - GetActorLocation()).Rotation();
				CC->CharacterChange(Pos, mArm->GetRelativeRotation(), Rot.Yaw);
				FinishDilation();
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.5);

				Destroy();
			}
			else
			{
				CC->CharacterChange(Pos, mArm->GetRelativeRotation(), AB->GetDirYaw());
				DisappearAnimation();
			}

			mDisappear = true;
			GetMesh()->SetRelativeRotation(FRotator(0, AB->GetDirYaw() - 90, 0));
			
		}
	}
}

void ACharacterBase::CharacterDodge(const FInputActionInstance& Instance)
{
	if (mShowUI)
		return;

	if (!mDodgeEnable)
		return;

	if (mDodge)
		return;

	mMoveEnable = false;
	mDodge = true;
	mSkillEnable = false;

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (!IsValid(AIB))
		return;

	if (mMoving)
	{
		mDodgeDir = mDodgeDirVector;
		AIB->SetDirYaw(mDodgeDir.Rotation().Yaw);
	}
	else
	{
		mDodgeDir = mDirScene->GetForwardVector() * (-1);
		AIB->SetDirYaw(mDirScene->GetForwardVector().Rotation().Yaw);
	}
	AIB->StopAttackMontage();
	AIB->StopAllMontages(0.1);
	AIB->SetDodge(true);

	if (mPDEnable)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		if (IsValid(ASC))
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.NoDamage")));

		AIB->SetAttackEnable(true);

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.25);
		GetWorld()->GetTimerManager().SetTimer(mDilationTimer, this, &ACharacterBase::FinishDilation, 0.25);
		mDodgeDir *= 0.3;

		mDodgeAfterImage->Activate(false);

		mDodgePostProcess->Settings.bOverride_SceneFringeIntensity = true;
		mDodgePostProcess->Settings.bOverride_VignetteIntensity = true;

		USoundBase* SB = LoadObject<USoundBase>(GetWorld(), TEXT("/Script/Engine.SoundWave'/Game/Sound/SW_PerfectDodge.SW_PerfectDodge'"));
		if (IsValid(SB))
			UGameplayStatics::PlaySound2D(GetWorld(), SB);
	}
}

void ACharacterBase::CharacterAttack(const FInputActionInstance& Instance)
{
	if (mShowUI)
		return;

	if (!mAttackEnable)
		return;
	/*
	if (mDodge)
	{
		DodgeAttack();
		mSkillEnable = true;
	}
	else
		ShortAttack();
	*/
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

	LongSkillTriggered();
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

	FinishDilation();

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
	}
}

void ACharacterBase::CharacterDisappear()
{
	Destroy();
}

void ACharacterBase::FinishDilation()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (IsValid(ASC))
		ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.NoDamage")));

	if (mUltimateDilation)
		return;

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	mDPPEnable = true;
	mMoveEnable = true;

}

void ACharacterBase::DodgeAttack()
{
	FinishDilation();
}

void ACharacterBase::SetCameraRotation(FRotator CameraRotation)
{
	mDirScene->SetRelativeRotation(FRotator(0,CameraRotation.Yaw,0));
	mArm->SetRelativeRotation(CameraRotation);
}

void ACharacterBase::SetDirYaw(float DirYaw)
{
	if (UAnimInstanceBase* AB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance()))
	{
		float RYaw = FMath::RandRange(-45, 45);
		AB->SetDirYaw(DirYaw);
		mDisappearDir = FRotator(0, DirYaw + RYaw, 0).Vector();
		mForward = true;
	}
}

void ACharacterBase::SetTargeting(AActor* Target)
{
	if (!mTargetArray.Contains(Target))
		mTargetArray.Add(Target);
}

void ACharacterBase::SetPDEnable(AActor* Target)
{
	mPDEnable = true;
	mTarget = Target;
	GetWorld()->GetTimerManager().SetTimer(mPDFail, this, &ACharacterBase::PDEnd, 0.5);
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

void ACharacterBase::CounterChange()
{
	ACharacterController* CC = GetController<ACharacterController>();
	if (!IsValid(CC))
		return;

	if (!CC->GetCounterEnable())
		return;

	mMoveEnable = false;
	CC->SetCounterEnable(false);
	mForward = false;
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayDodgeAndCounterAttack(1);

	UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Niagara/NS_Wave.NS_Wave'"));
	if (IsValid(NS))
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS, GetActorLocation());
	
	TArray<FHitResult> Results;
	bool Collision = GetWorld()->SweepMultiByProfile(Results, GetActorLocation(), GetActorLocation(), FQuat::Identity, TEXT("PlayerSkill"),
		FCollisionShape::MakeSphere(350));
	if (Collision)
	{
		ACharacterState* CS = GetPlayerState<ACharacterState>();
		for (int32 i = 0; i < Results.Num(); ++i)
		{
			AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
			if (IsValid(CS) && IsValid(Monster))
			{
				Monster->PlayKnockback(GetActorLocation());
				CS->PlayGE_CounterAttack(mName, Monster->GetAbilitySystemComponent());
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

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// FVector Dir = GetActorLocation() - DamageCauser->GetActorLocation();
	// Dir -= FVector(0, 0, Dir.Z);
	// Dir.Normalize();
	// 
	// LaunchCharacter(Dir * 1000 + FVector(0, 0, 200), false, false);
	// SetDirYaw((- Dir).Rotation().Yaw);

	return DamageAmount;
}
