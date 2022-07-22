// Copyright Epic Games, Inc. All Rights Reserved.

#include "mcPawn.h"
#include "mcBlock.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AmcPawn::AmcPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// 创建可附加内容的虚拟根组件。
	// RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// 创建相机和可见对象
	// UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	// OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	// 将相机和可见对象附加到根组件。偏移并旋转相机。
	// OurCamera->SetupAttachment(RootComponent);
	// OurCamera->SetRelativeLocation(FVector(-250.0f, 0.0f, 250.0f));
	// OurCamera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	// OurVisibleComponent->SetupAttachment(RootComponent);
}

void AmcPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
			{
				FVector Start = OurCamera->GetComponentLocation();
				FVector End = Start + (OurCamera->GetComponentRotation().Vector() * 8000.0f);
				TraceForBlock(Start, End, true);

				OurCamera->SetWorldLocation(CurrentVelocity);
			}
		}
		else
		{
			FVector Start, Dir, End;
			PC->DeprojectMousePositionToWorld(Start, Dir);
			End = Start + (Dir * 8000.0f);
			TraceForBlock(Start, End, false);
		}
	}
}

void AmcPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", EInputEvent::IE_Pressed, this, &AmcPawn::OnResetVR);
	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &AmcPawn::TriggerClick);
	PlayerInputComponent->BindAction("TriggerRightClick", EInputEvent::IE_Pressed, this, &AmcPawn::TriggerRightClick);

	// 对两个移动轴"MoveX"和"MoveY"的值逐帧反应。
	PlayerInputComponent->BindAxis("MoveX", this, &AmcPawn::MoveXAxis);
	PlayerInputComponent->BindAxis("MoveY", this, &AmcPawn::MoveYAxis);

}

void AmcPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, -90.0f, 0.0f);
}

void AmcPawn::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AmcPawn::TriggerClick()
{
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleClicked();
	}
}

void AmcPawn::TriggerRightClick()
{
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleRightClicked();
	}
}

void AmcPawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers)
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
	if (HitResult.Actor.IsValid())
	{
		AmcBlock* HitBlock = Cast<AmcBlock>(HitResult.Actor.Get());
		if (CurrentBlockFocus != HitBlock)
		{
			if (CurrentBlockFocus)
			{
				CurrentBlockFocus->Highlight(false);
			}
			if (HitBlock)
			{
				HitBlock->Highlight(true);
			}
			CurrentBlockFocus = HitBlock;
		}
	}
	else if (CurrentBlockFocus)
	{
		CurrentBlockFocus->Highlight(false);
		CurrentBlockFocus = nullptr;
	}
}

void AmcPawn::MoveXAxis(float AxisValue)
{
	// 以100单位/秒的速度向前或向后移动
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}

void AmcPawn::MoveYAxis(float AxisValue)
{
	// 以100单位/秒的速度向右或向左移动
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}
