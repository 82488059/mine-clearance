// Copyright Epic Games, Inc. All Rights Reserved.

#include "mcPlayerController.h"

AmcPlayerController::AmcPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AmcPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameAndUI());
}
