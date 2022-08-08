// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "mcPlayerController.generated.h"

/** PlayerController class used to enable cursor */
UCLASS()
class AmcPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AmcPlayerController();

	virtual void BeginPlay() override;
};


