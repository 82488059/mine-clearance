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
void AmcGameMode::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartingWidgetClass);
}

void AmcGameMode::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
    if (CurrentWidget != nullptr)
    {
        CurrentWidget->RemoveFromViewport();
        CurrentWidget = nullptr;
    }
    if (NewWidgetClass != nullptr)
    {
        CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
        if (CurrentWidget != nullptr)
        {
            CurrentWidget->AddToViewport();
        }
    }
}
