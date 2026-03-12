// Fill out your copyright notice in the Description page of Project Settings.


#include "Aurora.h"
#include "Algo/RandomShuffle.h"

AAurora::AAurora()
{
	ObjectFinder(USkeletalMesh, MeshAsset, "/Script/Engine.SkeletalMesh'/Game/ParagonAurora/Characters/Heroes/Aurora/Meshes/Aurora.Aurora'");
	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);

	GetCapsuleComponent()->SetCapsuleHalfHeight(90);
	GetCapsuleComponent()->SetCapsuleRadius(25);

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	ClassFinder(UAnimInstance, AnimAsset, "/Script/Engine.AnimBlueprint'/Game/Character/Aurora/ABP_Aurora.ABP_Aurora_C'");
	if (AnimAsset.Succeeded())
		GetMesh()->SetAnimClass(AnimAsset.Class);

	mUltimateEffect = MyObject(UNiagaraComponent, "UltimateEffect");

	ObjectFinder(UNiagaraSystem, EffectAsset, "/Script/Niagara.NiagaraSystem'/Game/Character/Aurora/NS_Aurora_Ultimate.NS_Aurora_Ultimate'");
	if (EffectAsset.Succeeded())
		mUltimateEffect->SetAsset(EffectAsset.Object);

	mUltimateEffect->SetupAttachment(GetMesh(), TEXT("Sword_Tip"));
	mUltimateEffect->SetRelativeScale3D(FVector(5));
	mUltimateEffect->bAutoActivate = false;

	mName = TEXT("Aurora");
		
	ClassFinder(UGameplayAbility, GA_Skill_Aurora, "/Script/Engine.Blueprint'/Game/GAS/GameplayAbility/Character/GA_Skill_Aurora.GA_Skill_Aurora_C'");
	if (GA_Skill_Aurora.Succeeded())
		mGA_Skill_Aurora = GA_Skill_Aurora.Class;

	ObjectFinder(USoundBase, SB_Attack, "/Script/Engine.SoundWave'/Game/Sound/SW_Aurora_Attack.SW_Aurora_Attack'");
	if (SB_Attack.Succeeded())
		mSB_Attack = SB_Attack.Object;

	ObjectFinder(USoundBase, SB_Skill, "/Script/Engine.SoundWave'/Game/Sound/SW_Aurora_Skill.SW_Aurora_Skill'");
	if (SB_Skill.Succeeded())
		mSB_Skill = SB_Skill.Object;

	ObjectFinder(UParticleSystem, PS, "/Script/Engine.ParticleSystem'/Game/ParagonAurora/FX/Particles/Abilities/Leap/FX/P_Aurora_Decoy_Frost.P_Aurora_Decoy_Frost'");
	if (PS.Succeeded())
		mUltimateParticle = PS.Object;

	mDisappearAnim = SoftPath(UAnimationAsset, "/Script/Engine.AnimSequence'/Game/ParagonAurora/Characters/Heroes/Aurora/Animations/Jog_Bwd.Jog_Bwd'");

	ObjectFinder(UNiagaraSystem, NS_Attack, "/Script/Niagara.NiagaraSystem'/Game/Character/Aurora/NS_Aurora_Attack.NS_Aurora_Attack'");
	if (NS_Attack.Succeeded())
		mNS_Attack = NS_Attack.Object;

	ObjectFinder(UParticleSystem, PS_Skill, "/Script/Engine.ParticleSystem'/Game/ParagonAurora/FX/Particles/Abilities/Freeze/FX/P_Aurora_Freeze_Whrilwind.P_Aurora_Freeze_Whrilwind'");
	if (PS_Skill.Succeeded())
		mPS_Skill = PS_Skill.Object;

	ObjectFinder(UNiagaraSystem, NS_Skill, "/Script/Niagara.NiagaraSystem'/Game/Character/Aurora/NS_Aurora_Skill.NS_Aurora_Skill'");
	if (NS_Skill.Succeeded())
		mNS_Skill = NS_Skill.Object;

	ObjectFinder(UNiagaraSystem, NS_AttackHit, "/Script/Niagara.NiagaraSystem'/Game/Character/Aurora/NS_Aurora_AttackHit.NS_Aurora_AttackHit'");
	if (NS_AttackHit.Succeeded())
		mNS_AttackHit = NS_AttackHit.Object;

	ObjectFinder(USoundBase, SB_AttackHit, "/Script/Engine.SoundWave'/Game/Sound/SW_Aurora_Attack_Hit.SW_Aurora_Attack_Hit'");
	if (SB_AttackHit.Succeeded())
		mSB_AttackHit = SB_AttackHit.Object;

	ObjectFinder(UParticleSystem, PS_SkillHit, "/Script/Engine.ParticleSystem'/Game/ParagonAurora/FX/Particles/Abilities/Primary/FX/P_Aurora_Melee_SucessfulImpact.P_Aurora_Melee_SucessfulImpact'");
	if (PS_SkillHit.Succeeded())
		mPS_SkillHit = PS_SkillHit.Object;

	ObjectFinder(USoundBase, SB_SkillHit, "/Script/Engine.SoundWave'/Game/Sound/SW_Aurora_Skill_Hit.SW_Aurora_Skill_Hit'");
	if (SB_SkillHit.Succeeded())
		mSB_SkillHit = SB_SkillHit.Object;

}

void AAurora::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mUltimating) // 궁극기 사용 중
	{
		float Height = GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("Height")); // 애니메이션 커브로 높이 가져오기
		SetActorLocation(mLocation + FVector(0, 0, Height * 5)); // 높이 설정
		mArm->TargetArmLength = mArmLength + Height * 7.5; // 카메라(스프링 암) 위치 설정

		mUTimer += DeltaTime;
		if (mUTimer > 0.08) // 0.08초마다 반복
		{
			if (mUCount < 8) // 궁극기 개수
			{
				mUTimer = 0;
				FRotator Rot = FRotator(0, mUArray[mUCount] * 360 / 8 + 22.5, 0); // 캐릭터 기준 8방향 각도 <- mUArray : 랜덤 배열
				FVector RandVec = UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(500, 0, 0), Rot) + GetActorLocation(); // 캐릭터 기준 벡터 회전
				FHitResult Result;
				GetWorld()->LineTraceSingleByChannel(Result, RandVec + FVector(0, 0, 10000), RandVec - FVector(0, 0, 10000), ECollisionChannel::ECC_Camera); // 라인 트레이스 -> 바닥 찾기
				if (IsValid(mUltimateParticle)) // 궁극기 이펙트
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mUltimateParticle, Result.Location, FRotator::ZeroRotator, FVector(3));
				mUCount += 1;

				TArray<FHitResult> Results;
				bool Collision = GetWorld()->SweepMultiByProfile(Results, Result.Location, Result.Location, FQuat::Identity, TEXT("PlayerSkill"),
					FCollisionShape::MakeSphere(300)); // 위에서 찾은 바닥에 구 트레이스

				for(int32 i=0; i<Results.Num();++i)
					GiveDamage(Results[i].GetActor(), 10, false); // 맞은 대상 대미지 주기
			}
		}
	}

}

void AAurora::ShortSkill() // Check - CharacterState 이후
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if(IsValid(ASC) && mGA_Skill_Aurora)
		ASC->TryActivateAbilityByClass(mGA_Skill_Aurora);
}

void AAurora::Ultimate()
{
	if (IsValid(mAnimInstance))
	{
		mAnimInstance->PlayUltimate(0); // 궁극기 애니메이션
		mAnimInstance->SetDirYaw(mDirScene->GetRelativeRotation().Yaw); // 항상 정면을 바라보게
	}

	mLocation = GetActorLocation(); // 현재 위치 저장 -> 캐릭터의 높이 조절 용
	mArmLength = mArm->TargetArmLength; // 현재 스프링 암 길이 저장 -> 카메라 시야 조절 용
	mUltimating = true; // 궁극기 사용 중임을 알림
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.25); // 슬로우 모션
	GetWorld()->GetTimerManager().SetTimer(mUltimateTimer, this, &AAurora::FinishUltimateDilation, 1); // 1초 후 슬로우모션 종료

	mUltimateEffect->Activate(false); // 궁극기 이펙트 시작
	mUltimateDilation = true; // 궁극기 슬로우 모션

	mUCount = 0; // 궁극기 카운트 -> 8 방향
	mUArray.Empty(); // 배열 초기화

	for (int32 i = 0; i < 8; ++i)
		mUArray.Add(i);

	Algo::RandomShuffle(mUArray); // 셔플

}

void AAurora::DodgeAttack()
{
	Super::DodgeAttack();
	UAnimInstanceBase* AIB = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance());
	if (IsValid(AIB))
		AIB->PlayDodgeAndCounterAttack(0);
}

void AAurora::ShortAttackEffect(int32 Index, float Yaw)
{
	if (IsValid(mNS_Attack)) // 공격 용 나이아가라
	{
		FVector Pos = GetActorLocation() + FVector(0, 0, 40); // 이펙트 실행 장소
		FRotator Rot = FRotator(0, Yaw, 0); // 현재 캐릭터가 바라보고 있는 방향 각도
		switch (Index) // 콤보 공격에 따른 방향 조절
		{
		case 0:
			Rot += FRotator(0, 180, 20);
			break;
		case 1:
			Rot += FRotator(0, 180, 135);
			break;
		case 2:
			Rot += FRotator(0, 180, 45);
			break;
		case 3:
			Rot += FRotator(0, 190, 240);
			break;
		}
		UNiagaraFunctionLibrary::SpawnSystemAttached(mNS_Attack, RootComponent, FName(TEXT("")), Pos, Rot, FVector(1.5),
			EAttachLocation::KeepWorldPosition, true, ENCPoolMethod::AutoRelease);

		if (IsValid(mSB_Attack)) // 공격 소리
			UGameplayStatics::PlaySound2D(GetWorld(), mSB_Attack);
	}
}

void AAurora::ShortSkillEffect(int32 Index, float Yaw)
{
	if (IsValid(mPS_Skill) && IsValid(mNS_Skill)) // 스킬 용 파티클 시스템
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mPS_Skill, GetActorLocation()); // 파티클 재생

		FVector Pos = GetActorLocation() + FVector(0, 0, 20); // 나이아가라 시스템 위치 조절
		FRotator Rot = FRotator(0, Yaw + 200, 10);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), mNS_Skill, Pos, Rot, FVector(2.5));

		if (IsValid(mSB_Skill)) // 스킬 소리
			UGameplayStatics::PlaySound2D(GetWorld(), mSB_Skill);
	}
}

void AAurora::RealAttack(int32 Index)
{
	TArray<FHitResult> Results;
	float Yaw = 0;
	if (IsValid(mAnimInstance))
		Yaw = mAnimInstance->GetDirYaw(); // 캐릭터가 바라보고 있는 방향
	FVector Pos = GetActorLocation() + UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(50, 0, 0), FRotator(0, Yaw, 0));
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(200)); // 공격 범위

	if (Collision)
	{
		if (IsValid(mNS_AttackHit))
		{
			ACharacterState* CS = GetPlayerState<ACharacterState>();
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				FVector Pos2 = GetActorLocation() + (Results[i].GetActor()->GetActorLocation() - GetActorLocation()) / 2;
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), mNS_AttackHit,
					Pos2, FRotator::ZeroRotator, FVector(10 + Index*100));

				AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
				if (IsValid(CS) && IsValid(Monster)) // 공격 효과는 Gameplay Effect 사용
					CS->PlayGE_Attack(mName, Monster->GetAbilitySystemComponent());

				if (IsValid(mSB_AttackHit))
					UGameplayStatics::PlaySound2D(GetWorld(), mSB_AttackHit);
			}
		}
	}
}

void AAurora::RealSkill()
{
	TArray<FHitResult> Results;
	FVector Pos = GetActorLocation();
	Pos -= FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(750));

	if (Collision)
	{
		if (IsValid(mPS_SkillHit))
		{
			ACharacterState* CS = GetPlayerState<ACharacterState>();
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mPS_SkillHit,
					Results[i].GetActor()->GetActorLocation(), FRotator::ZeroRotator, FVector(5));

				AMonsterBase* Monster = Cast<AMonsterBase>(Results[i].GetActor());
				if (IsValid(CS)&&IsValid(Monster))
					CS->PlayGE_Skill(mName, Monster->GetAbilitySystemComponent());

				if (IsValid(mSB_SkillHit))
					UGameplayStatics::PlaySound2D(GetWorld(), mSB_SkillHit);
			}
		}
	}
}

void AAurora::RealDodgeAttack()
{
	TArray<FHitResult> Results;
	float Yaw = Cast<UAnimInstanceBase>(GetMesh()->GetAnimInstance())->GetDirYaw();
	FVector Pos = GetActorLocation() + UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(50, 0, 0), FRotator(0, Yaw, 0));
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Pos, Pos, FQuat::Identity,
		TEXT("PlayerSkill"), FCollisionShape::MakeSphere(200));

	if (Collision)
	{
		UNiagaraSystem* NS = LoadObject<UNiagaraSystem>(GetWorld(), TEXT("/Script/Niagara.NiagaraSystem'/Game/Character/Aurora/NS_Aurora_AttackHit.NS_Aurora_AttackHit'"));
		if (IsValid(NS))
		{
			for (int32 i = 0; i < Results.Num(); ++i)
			{
				FVector Pos2 = GetActorLocation() + (Results[i].GetActor()->GetActorLocation() - GetActorLocation()) / 2;
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS,
					Pos2, FRotator::ZeroRotator, FVector(10));

				GiveDamage(Results[i].GetActor(), 10, false);
			}
		}
	}
}

void AAurora::RealUltimate()
{
	FHitResult Result;
	GetWorld()->LineTraceSingleByChannel(Result, GetActorLocation(), GetActorLocation() - FVector(0, 0, 10000), ECollisionChannel::ECC_Camera);

	TArray<FHitResult> Results;
	bool Collision = GetWorld()->SweepMultiByProfile(Results, Result.Location, Result.Location, FQuat::Identity, TEXT("PlayerSkill"),
		FCollisionShape::MakeSphere(1000));

	for(int32 i=0; i<Results.Num();++i)
		GiveDamage(Results[i].GetActor(), 10, true);
}

void AAurora::FinishUltimateDilation()
{
	mUltimating = false;
	mArm->TargetArmLength = mArmLength;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);

	mUltimateEffect->Deactivate();

	mUltimateDilation = false;

	mUltimateEnable = true;
	mDodgeEnable = true;
}
