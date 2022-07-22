// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "mcPawn.generated.h"

UCLASS(config=Game)
class AmcPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

protected:
	void OnResetVR();
	void TriggerClick();
	void TriggerRightClick();
	
	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	void MoveXAxis(float AxisValue);
	void MoveYAxis(float AxisValue);

	//  ‰»Î±‰¡ø
	FVector CurrentVelocity;
	bool bGrowing;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class AmcBlock* CurrentBlockFocus;
};
