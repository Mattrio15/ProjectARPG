// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBase.h"
#include "MonsterAnimInstance.h"
#include "../../Character/CharacterBase.h"
#include "../../UI/Monster/MonsterHPWidget.h"
#include "../../MyGameInstance.h"

UDataTable* AMonsterBase::mMonsterInfoTable = nullptr;

AMonsterBase::AMonsterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AMonsterController::StaticClass();

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Monster"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UAttributeDataAsset>
		DataAsset(TEXT("/Script/ProjectARPG.AttributeDataAsset'/Game/GAS/DA_AttributeTag.DA_AttributeTag'"));
	if (DataAsset.Succeeded())
		mADA = DataAsset.Object;

	mASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	mASC->SetIsReplicated(true);
	mASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	mMAS = CreateDefaultSubobject<UMonsterAttributeSet>(TEXT("MAS"));
	
	static ConstructorHelpers::FClassFinder<UGameplayEffect>
		InitEffectAsset(TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayEffect/Monster/GE_MonsterInit.GE_MonsterInit_C'"));
	if (InitEffectAsset.Succeeded())
		mGE_MonsterInit = InitEffectAsset.Class;

	static ConstructorHelpers::FClassFinder<UGameplayEffect>
		DamageEffectAsset(TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayEffect/GE_Damage.GE_Damage_C'"));
	if (DamageEffectAsset.Succeeded())
		mGE_MonsterDamage = DamageEffectAsset.Class;

	static ConstructorHelpers::FClassFinder<UGameplayEffect>
		ElementalEffectAsset(TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayEffect/Character/Skill/ElementalEffect/GE_ElementalOver.GE_ElementalOver_C'"));
	if (ElementalEffectAsset.Succeeded())
		mGE_ElementalOver = ElementalEffectAsset.Class;

	mMonsterHPWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MonsterHPWidget"));
	mMonsterHPWidget->SetupAttachment(RootComponent);
	mMonsterHPWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UMonsterHPWidget>
		WidgetAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Monster/WB_MonsterHP.WB_MonsterHP_C'"));
	if (WidgetAsset.Succeeded())
		mMonsterHPWidget->SetWidgetClass(WidgetAsset.Class);

	mMonsterHPWidget->SetRelativeLocation(FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	static ConstructorHelpers::FObjectFinder<UElementalDataAsset>
		EDA(TEXT("/Script/ProjectARPG.ElementalDataAsset'/Game/GAS/GameplayEffect/Character/Skill/DA_ElementalEffectData.DA_ElementalEffectData'"));
	if (EDA.Succeeded())
		mEDA = EDA.Object;
		
}

ETeamAttitude::Type AMonsterBase::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other);

	if (!OtherTeamAgent)
		return ETeamAttitude::Neutral;
	if (OtherTeamAgent->GetGenericTeamId().GetId() == 255)
		return ETeamAttitude::Neutral;
	return OtherTeamAgent->GetGenericTeamId() == mTeamId ? ETeamAttitude::Friendly : ETeamAttitude::Hostile;
}

UAbilitySystemComponent* AMonsterBase::GetAbilitySystemComponent() const
{
	return mASC;
}

void AMonsterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!IsValid(mMonsterInfoTable))
		mMonsterInfoTable = LoadObject<UDataTable>(GetWorld(), TEXT("/Script/Engine.DataTable'/Game/GAS/GameplayEffect/Monster/DT_MonsterInfo.DT_MonsterInfo'"));

	FMonsterInfo* Info = mMonsterInfoTable->FindRow<FMonsterInfo>(mName, TEXT(""));

	mASC->InitAbilityActorInfo(this, this);

	FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
	FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(mGE_MonsterInit, 1, Context);
	for (auto& Data : Info->MonsterInfo)
		Spec.Data->SetSetByCallerMagnitude(Data.Key, Data.Value);

	mASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());

	GetCharacterMovement()->MaxWalkSpeed = Info->MonsterInfo[mADA->mMonsterMoveSpeedTag];

}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	UMonsterHPWidget* MonsterHPWidget = Cast<UMonsterHPWidget>(mMonsterHPWidget->GetWidget());
	if (IsValid(MonsterHPWidget))
	{
		MonsterHPWidget->SetHPBar(1);
		mMAS->OnHealthChanged.AddUObject(MonsterHPWidget, &UMonsterHPWidget::SetHPBar);
		mMAS->OnElementalChanged.AddUObject(MonsterHPWidget, &UMonsterHPWidget::SetElementalBar);
		mMAS->OnHitDamage.AddUObject(MonsterHPWidget, &UMonsterHPWidget::SetTextDamage);
	}

	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); ++i)
	{
		UMaterialInstanceDynamic* MID = GetMesh()->CreateDynamicMaterialInstance(i);
		mMIDArray.Add(MID);
	}

	mMAS->OnMoveSpeedChanged.AddUObject(this, &AMonsterBase::SetMoveSpeed);
	mMAS->OnElementalOver.AddUObject(this, &AMonsterBase::PlayGE_ElementalOver);

	UMyGameInstance* GI = GetGameInstance<UMyGameInstance>();
	if (IsValid(GI))
	{
		mPlayerController = GI->GetFirstLocalPlayerController();
	}
}

void AMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(mPlayerController))
	{
		APawn* Pawn = mPlayerController->GetPawn();
		if (IsValid(Pawn))
		{
			FVector PlayerPos = mPlayerController->GetPawn()->GetActorLocation();
			FVector MonsterPos = GetActorLocation();
			float Dis = (PlayerPos - MonsterPos).Length();
			if (Dis > 3000)
				mMonsterHPWidget->SetVisibility(false);
			else
				mMonsterHPWidget->SetVisibility(true);
		}
	}
}

void AMonsterBase::CriticalHit(FVector Dir)
{
	if (!GetController<AMonsterController>()->GetBlackboardComponent()->GetValueAsBool(TEXT("CriticalHit")))
	{
		Dir -= FVector(0, 0, Dir.Z);
		Dir.Normalize();
		LaunchCharacter(Dir * 1000 + FVector(0, 0, 200), false, false);
	}

	GetController<AMonsterController>()->GetBlackboardComponent()->SetValueAsBool(TEXT("CriticalHit"), true);
}

void AMonsterBase::SetIsHit(bool A)
{
	UMonsterAnimInstance* MAI = Cast<UMonsterAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(MAI))
		MAI->SetIsHit(A);
}

void AMonsterBase::Death(FVector Pos)
{
	GetController<AMonsterController>()->GetBrainComponent()->StopLogic(TEXT("Death"));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Spectator"));

	UMonsterAnimInstance* MAI = Cast<UMonsterAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(MAI))
		MAI->SetIsDeath(true);

	FVector Dir = GetActorLocation() - Pos;
	Dir -= FVector(0, 0, Dir.Z);
	Dir.Normalize();
	LaunchCharacter(Dir * 1000 + FVector(0, 0, 500), true, false);


	TSubclassOf<UGameplayEffect> GE_Death = LoadClass<UGameplayEffect>(GetWorld(), TEXT("/Script/Engine.Blueprint'/Game/GAS/GameplayEffect/Monster/GE_MonsterDeath.GE_MonsterDeath_C'"));
	if (IsValid(GE_Death))
	{
		FGameplayEffectContextHandle Context = mASC->MakeEffectContext();
		FGameplayEffectSpecHandle Spec = mASC->MakeOutgoingSpec(GE_Death, 1, Context);
		mASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}

	GetWorld()->GetTimerManager().SetTimer(mDissolveTimer, this, &AMonsterBase::Dissolve, 0.01, true);
}

void AMonsterBase::SetElemental(FGameplayTag Tag)
{
	mASC->RemoveLooseGameplayTag(mADA->mElemental_IceTag);
	mASC->RemoveLooseGameplayTag(mADA->mElemental_DarkTag);
	mASC->RemoveLooseGameplayTag(mADA->mElemental_ElectricTag);
	mASC->RemoveLooseGameplayTag(mADA->mElemental_BleedTag);
	mASC->RemoveLooseGameplayTag(mADA->mElemental_LightTag);
	mASC->RemoveLooseGameplayTag(mADA->mElemental_FireTag);

	mASC->AddLooseGameplayTag(Tag);

	UMonsterHPWidget* MonsterWidget = Cast<UMonsterHPWidget>(mMonsterHPWidget->GetWidget());
	if (IsValid(MonsterWidget))
	{
		for (FElementalEffectData& Data : mEDA->mElementalEffectDataArray)
		{
			if (mASC->HasMatchingGameplayTag(Data.ElementalTag))
				MonsterWidget->SetElementalTexture(Data.ElementalTexture);
		}
	}
}

void AMonsterBase::PlayKnockback(FVector Start)
{
	AMonsterController* MC = GetController<AMonsterController>();
	if (IsValid(MC))
	{
		if (MC->GetBlackboardComponent()->GetValueAsBool(TEXT("CriticalHit")))
			return;
		MC->GetBlackboardComponent()->SetValueAsBool(TEXT("CriticalHit"), true);
	}

	FVector Dir = GetActorLocation() - Start;						// 넉백 방향
	Dir -= FVector(0, 0, Dir.Z);									// 높이 빼기
	float Speed = 800;												// 날아갈 속도
	float Angle = FMath::Tan(PI/4);									// 날아갈 각도
	FVector Velocity = Dir + FVector(0, 0, Dir.Length()*Angle);		// 날아갈 방향
	Velocity.Normalize();											// 단위화
	LaunchCharacter(Velocity * Speed, true, true);					// 발사
}

void AMonsterBase::Dissolve()
{
	mDissolveValue += 0.01;
	for (int32 i = 0; i < mMIDArray.Num(); ++i)
		mMIDArray[i]->SetScalarParameterValue(TEXT("Dissolve"), mDissolveValue);
	if (mDissolveValue >= 1.5)
	{
		GetWorld()->GetTimerManager().ClearTimer(mDissolveTimer);
		Destroy();
	}
}

void AMonsterBase::SetMoveSpeed(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AMonsterBase::PlayGE_ElementalOver(UAbilitySystemComponent* Source)
{
	FGameplayEffectContextHandle Context = Source->MakeEffectContext();
	if (IsValid(mGE_ElementalOver)) // 속성 수치 초과 시 감소
	{
		FGameplayEffectSpecHandle Spec = Source->MakeOutgoingSpec(mGE_ElementalOver, 1, Context);
		Source->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), mASC);
	}

	for (FElementalEffectData& Data : mEDA->mElementalEffectDataArray) // 속성 수치 초과 시 실제 효과 적용
	{
		if (mASC->HasMatchingGameplayTag(Data.ElementalTag))
		{
			if (IsValid(Data.ElementalEffect))
			{
				FGameplayEffectSpecHandle Spec = Source->MakeOutgoingSpec(Data.ElementalEffect, 1, Context);
				Source->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), mASC);
			}
		}
	}

}
