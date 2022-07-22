// Copyright Epic Games, Inc. All Rights Reserved.

#include "mcGameMode.h"
#include "mcPlayerController.h"
#include "mcPawn.h"

AmcGameMode::AmcGameMode()
{
	// no pawn by default
	DefaultPawnClass = AmcPawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = AmcPlayerController::StaticClass();
}
