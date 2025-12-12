// Fill out your copyright notice in the Description page of Project Settings.


#include "Muriel.h"
#include "Muriel_Attack.h"
#include "Muriel_Ultimate.h"

AMuriel::AMuriel()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonMuriel/Characters/Heroes/Muriel/Meshes/Muriel_GDC.Muriel_GDC'"));
	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);

	GetCapsuleComponent()->SetCapsuleHalfHeight(105);
	GetCapsuleComponent()->SetCapsuleRadius(25);

	GetMesh()->SetRelativeLocation(FVector(0, 0, -105));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Character/Muriel/ABP_Muriel.ABP_Muriel_C'"));
	if (AnimAsset.Succeeded())
		GetMesh()->SetAnimClass(AnimAsset.Class);

	mSkillRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SkillRoot"));
	mSkillC = CreateDefaultSubobject<USceneComponent>(TEXT("SkillC"));
	mSkillRoot->SetupAttachment(RootComponent);
	mSkillC->SetupAttachment(mSkillRoot);

	mSkillC->SetRelativeLocation(FVector(200, 0, 75));

	mSkillEffectBase = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffectBase"));
	mSkillEffect00 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffect00"));
	mSkillEffect01 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffect01"));
	mSkillEffect02 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffect02"));
	mSkillEffect03 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffect03"));
	mSkillEffect04 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffect04"));
	mSkillEffect05 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffect05"));
	mSkillEffect06 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffect06"));
	mSkillEffect07 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffect07"));
	mSkillEffect08 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillEffect08"));
	mSkillEffectBase->SetupAttachment(mSkillC);
	mSkillEffect00->SetupAttachment(mSkillC);
	mSkillEffect01->SetupAttachment(GetMesh(), TEXT("Skill01"));
	mSkillEffect02->SetupAttachment(GetMesh(), TEXT("Skill02"));
	mSkillEffect03->SetupAttachment(GetMesh(), TEXT("Skill03"));
	mSkillEffect04->SetupAttachment(GetMesh(), TEXT("Skill04"));
	mSkillEffect05->SetupAttachment(GetMesh(), TEXT("Skill05"));
	mSkillEffect06->SetupAttachment(GetMesh(), TEXT("Skill06"));
	mSkillEffect07->SetupAttachment(GetMesh(), TEXT("Skill07"));
	mSkillEffect08->SetupAttachment(GetMesh(), TEXT("Skill08"));
	mSkillEffectArray.Add(mSkillEffect01);
	mSkillEffectArray.Add(mSkillEffect02);
	mSkillEffectArray.Add(mSkillEffect03);
	mSkillEffectArray.Add(mSkillEffect04);
	mSkillEffectArray.Add(mSkillEffect05);
	mSkillEffectArray.Add(mSkillEffect06);
	mSkillEffectArray.Add(mSkillEffect07);
	mSkillEffectArray.Add(mSkillEffect08);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		EffectAsset02(TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Muriel/NS_Muriel_Skill.NS_Muriel_Skill'"));
	if (EffectAsset02.Succeeded())
		mSkillEffectBase->SetAsset(EffectAsset02.Object);
	mSkillEffectBase->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		EffectAsset01(TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Muriel/NS_Muriel_Skill03.NS_Muriel_Skill03'"));
	if (EffectAsset01.Succeeded())
		mSkillEffect00->SetAsset(EffectAsset01.Object);
	mSkillEffect00->bAutoActivate = false;
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		EffectAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Muriel/NS_Muriel_Skill02.NS_Muriel_Skill02'"));
	if (EffectAsset.Succeeded())
	{
		for (int32 i = 0; i < 8; ++i)
		{
			mSkillEffectArray[i]->SetAsset(EffectAsset.Object);
			mSkillEffectArray[i]->bAutoActivate = false;
		}
	}

	mUltimateEffect01 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("UltimateEffect01"));
	mUltimateEffect02 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("UltimateEffect02"));
	mUltimateEffect01->SetupAttachment(GetMesh(), TEXT("AttackL"));
	mUltimateEffect02->SetupAttachment(GetMesh(), TEXT("AttackR"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		UltimateEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Muriel/NS_Muriel_Ultimate2.NS_Muriel_Ultimate2'"));
	if (UltimateEffect.Succeeded())
	{
		mUltimateEffect01->SetAsset(UltimateEffect.Object);
		mUltimateEffect02->SetAsset(UltimateEffect.Object);
		mUltimateEffect01->bAutoActivate = false;
		mUltimateEffect02->bAutoActivate = false;
	}

	static ConstructorHelpers::FClassFinder<UGameplayAbility>
		GA_Attack_Muriel(TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayAbility/Character/GA_Attack_Muriel.GA_Attack_Muriel_C'"));
	if (GA_Attack_Muriel.Succeeded())
		mGA_Attack_Muriel = GA_Attack_Muriel.Class;

	static ConstructorHelpers::FClassFinder<UGameplayAbility>
		GA_Skill_Muriel(TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayAbility/Character/GA_Skill_Muriel.GA_Skill_Muriel_C'"));
	if (GA_Skill_Muriel.Succeeded())
		mGA_Skill_Muriel = GA_Skill_Muriel.Class;

	mName = TEXT("Muriel");
}

void AMuriel::BeginPlay()
{
	Super::BeginPlay();
	MoveAttack();
}

void AMuriel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mAttackReady)
	{
		UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
		if (IsValid(AIB))
		{
			AIB->SetDirYaw(mDirScene->GetRelativeRotation().Yaw);
			mAttackTimer += DeltaTime;
			if (mAttackTimer > 1000)
			{
				mAttackReady = false;
				AIB->SetAttackReady(mAttackReady);
				mAttackTimer = 0;
				mAttackCount = 0;
			}
		}
	}

	if (mSkilling)
	{
		mSkillRoot->SetRelativeRotation(mDirScene->GetRelativeRotation());

		FVector End = mSkillC->GetComponentLocation() + mSkillC->GetForwardVector() * 10000;
		mSkillEffect00->SetVariableVec3(TEXT("User.End"), End);
		mSkillEffect00->SetVariableVec3(TEXT("User.Acceleration"), mSkillC->GetForwardVector() * 20000);

		for (int32 i = 0; i < 8; ++i)
		{
			mSkillEffectArray[i]->SetVariableVec3(TEXT("User.End"), mSkillC->GetComponentLocation());
			FVector Skill;
			if(i<4)
				Skill = GetMesh()->GetSocketLocation(FName(*FString::Printf(TEXT("Skill0%d"), i + 1))) - GetMesh()->GetSocketLocation(TEXT("AttackL"));
			else
				Skill = GetMesh()->GetSocketLocation(FName(*FString::Printf(TEXT("Skill0%d"), i + 1))) - GetMesh()->GetSocketLocation(TEXT("AttackR"));
			Skill.Normalize();
			mSkillEffectArray[i]->SetVariableVec3(TEXT("User.StartTan"), Skill);
			mSkillEffectArray[i]->SetVariableVec3(TEXT("User.EndTan"), Skill);
		}
	}

	if (mUltimating)
	{
		float Height = GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("Height"));
		SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, mVecZ + Height * 10));

		if (GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("MoveEnable")) > 0.5)
		{
			float Forward = GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("Forward"));
			GetCharacterMovement()->MaxWalkSpeed = mWalkSpeed * Forward;
			GetCharacterMovement()->MaxAcceleration = mAcceleration * Forward;
			AddMovementInput(mDirScene->GetForwardVector(), 1);
		}

		float ArmLen = GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("Camera"));
		mArm->TargetArmLength = mArmLen + ArmLen * 10;
	}
}

void AMuriel::DisappearAnimation()
{
	UAnimationAsset* AA = LoadObject<UAnimationAsset>(GetWorld(), TEXT("/Script/Engine.AnimSequence'/Game/ParagonMuriel/Characters/Heroes/Muriel/Animations/Jog_Bwd.Jog_Bwd'"));
	if (IsValid(AA))
		GetMesh()->PlayAnimation(AA, true);
}

void AMuriel::MoveAttack()
{
	if (mAttackReady)
	{
		mAttackTimer = 0;
		mAttackCount += 1;

		return;
	}
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
	{
		mAttackReady = true;
		AIB->SetAttackReady(mAttackReady);

		mMoveAttackEnable = true;
	}
}

void AMuriel::LongAttack()
{
	if (!mMoveAttackEnable)
		return;

	if (mAttackCount == 0)
		return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (IsValid(ASC) && IsValid(mGA_Attack_Muriel))
		ASC->TryActivateAbilityByClass(mGA_Attack_Muriel);

}

void AMuriel::LongSkillReady()
{
	if (!mAttackReady)
		return;

	mSkilling = true;
	mAttackEnable = false;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (IsValid(ASC) && IsValid(mGA_Skill_Muriel))
		ASC->TryActivateAbilityByClass(mGA_Skill_Muriel);

}

void AMuriel::LongSkillCompleted()
{
	mDodgeEnable = true;

	mSkilling = false;
	mAttackEnable = true;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (IsValid(ASC) && IsValid(mGA_Skill_Muriel))
	{
		FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(mGA_Skill_Muriel);
		ASC->CancelAbilityHandle(Spec->Handle);
	}
}

void AMuriel::Ultimate()
{
	mAttackEnable = false;
	mSkillEnable = false;
	mMoveEnable = false;
	
	mWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	mAcceleration = GetCharacterMovement()->MaxAcceleration;

	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayUltimate(0);

	GetWorld()->GetTimerManager().SetTimer(mUltimateTimer, this, &AMuriel::UltimateEnd, 4.3);
	mVecZ = GetActorLocation().Z;

	mUltimating = true;
	mArmLen = mArm->TargetArmLength;

	mUltimateEffect01->Activate(false);
	mUltimateEffect02->Activate(false);

	UParticleSystem* PS = LoadObject<UParticleSystem>(GetWorld(), TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMuriel/FX/Particles/Abilities/Ultimate/FX/P_FortuneReverse_KnockUp.P_FortuneReverse_KnockUp'"));
	if (IsValid(PS))
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS, GetMesh()->GetComponentLocation(), FRotator::ZeroRotator, true);

}

void AMuriel::UltimateEffect()
{
	AMuriel_Ultimate* MUL = GetWorld()->SpawnActor<AMuriel_Ultimate>(AMuriel_Ultimate::StaticClass(),
		GetMesh()->GetSocketLocation(TEXT("AttackL")), GetMesh()->GetSocketRotation(TEXT("AttackL")));
	AMuriel_Ultimate* MUR = GetWorld()->SpawnActor<AMuriel_Ultimate>(AMuriel_Ultimate::StaticClass(),
		GetMesh()->GetSocketLocation(TEXT("AttackR")), GetMesh()->GetSocketRotation(TEXT("AttackR")));
	if (IsValid(MUL) && IsValid(MUR))
	{
		float Speed1 = FMath::RandRange(10000, 15000); // 발사 속도 조절
		float Speed2 = FMath::RandRange(10000, 15000);
		MUL->SetHomingTarget(RootComponent); // 호밍 대상 설정
		MUR->SetHomingTarget(RootComponent);
		MUL->SetSpeed(Speed1); // 발사 속도 설정
		MUR->SetSpeed(Speed2);
		MUL->SetParent(this); // 공격자(캐릭터) 설정
		MUR->SetParent(this);
	}
}

void AMuriel::UltimateEnd()
{
	mAttackEnable = true;
	mSkillEnable = true;
	mMoveEnable = true;
	mDodgeEnable = true;

	mUltimateEnable = true;
	mUltimating = false;
	mArm->TargetArmLength = mArmLen;

	mUltimateEffect01->Deactivate();
	mUltimateEffect02->Deactivate();
}

FVector AMuriel::GetDirSceneVector()
{
	FVector Vec = mDirScene->GetForwardVector();
	return Vec;
}

FVector AMuriel::GetEndVector()
{
	FVector Vec = mArm->GetForwardVector() * 10000 + GetActorLocation();
	return Vec;
}

void AMuriel::SkillEffectOn(int32 Index)
{
	switch (Index)
	{
	case 0:
		mSkillEffectBase->Activate(false);

		for (int32 i = 0; i < 8; ++i)
			mSkillEffectArray[i]->Activate(false);

		break;
	case 1:
		mSkillEffect00->Activate(false);

		break;
	case 2:
		mSkillEffectBase->Deactivate();
		mSkillEffect00->Deactivate();

		for (int32 i = 0; i < 8; ++i)
			mSkillEffectArray[i]->Deactivate();

		break;
	}
}
