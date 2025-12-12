// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameMode.h"
#include "../Character/CharacterController.h"
#include "../Character/CharacterState.h"
#include "../Character/C_Aurora/Aurora.h"
#include "../Character/C_Kallari/Kallari.h"
#include "../Character/C_Muriel/Muriel.h"
#include "../Character/C_Revenant/Revenant.h"
#include "../Character/C_Terra/Terra.h"
#include "../Character/C_TwinBlast/TwinBlast.h"


ADefaultGameMode::ADefaultGameMode()
{
	DefaultPawnClass = AAurora::StaticClass();
	PlayerControllerClass = ACharacterController::StaticClass();
	PlayerStateClass = ACharacterState::StaticClass();
}

// AAurora AKallari AMuriel ARevenant ATerra ATwinBlast