// Fill out your copyright notice in the Description page of Project Settings.


#include "InputData.h"

UInputData::UInputData()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>
		Context(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Default.IMC_Default'"));
	if (Context.Succeeded())
		mMappingContext = Context.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		Move(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Move.IA_Move'"));
	if (Move.Succeeded())
		mMove = Move.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		CameraRotation(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_CameraRotation.IA_CameraRotation'"));
	if (CameraRotation.Succeeded())
		mCameraRotation = CameraRotation.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		CharacterChange(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_CharacterChange.IA_CharacterChange'"));
	if (CharacterChange.Succeeded())
		mCharacterChange = CharacterChange.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		Dodge(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Dodge.IA_Dodge'"));
	if (Dodge.Succeeded())
		mDodge = Dodge.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		Attack(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Attack.IA_Attack'"));
	if (Attack.Succeeded())
		mAttack = Attack.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		Skill(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Skill.IA_Skill'"));
	if (Skill.Succeeded())
		mSkill = Skill.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		Ultimate(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Ultimate.IA_Ultimate'"));
	if (Ultimate.Succeeded())
		mUltimate = Ultimate.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		Inventory(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Inventory.IA_Inventory'"));
	if (Inventory.Succeeded())
		mInventory = Inventory.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		QuickSlot1(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_QuickSlot_1.IA_QuickSlot_1'"));
	if (QuickSlot1.Succeeded())
		mQuickSlot_1 = QuickSlot1.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		QuickSlot2(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_QuickSlot_2.IA_QuickSlot_2'"));
	if (QuickSlot2.Succeeded())
		mQuickSlot_2 = QuickSlot2.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		QuickSlot3(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_QuickSlot_3.IA_QuickSlot_3'"));
	if (QuickSlot3.Succeeded())
		mQuickSlot_3 = QuickSlot3.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		Pause(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Pause.IA_Pause'"));
	if (Pause.Succeeded())
		mPause = Pause.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		FKey(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_FKey.IA_FKey'"));
	if (FKey.Succeeded())
		mFKey = FKey.Object;

	ObjectFinder(UInputAction, Status, "/Script/EnhancedInput.InputAction'/Game/Input/IA_Status.IA_Status'");
	if (Status.Succeeded())
		mStatus = Status.Object;

}
