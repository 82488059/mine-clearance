// Copyright Epic Games, Inc. All Rights Reserved.

#include "mcPawn.h"
#include "mcBlock.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"


AmcPawn::AmcPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// 创建可附加内容的虚拟根组件。
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// 创建相机和可见对象
	 //UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	 //OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	 // 将相机和可见对象附加到根组件。偏移并旋转相机。
	 //OurCamera->SetupAttachment(RootComponent);
	 //OurCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 1200.0f));
	 //OurCamera->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	 //OurVisibleComponent->SetupAttachment(RootComponent);
	// 使用弹簧臂给予摄像机平滑自然的运动感。
	//USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	//SpringArm->SetupAttachment(RootComponent);
	//SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	//SpringArm->TargetArmLength = 400.0f;
	//SpringArm->bEnableCameraLag = true;
	//SpringArm->CameraLagSpeed = 3.0f;

	// 创建摄像机并附加到弹簧臂
	//UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ActualCamera"));
	//Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CurrentVelocity.X = 0.f;
	CurrentVelocity.Y = 0.f;
	CurrentVelocity.Z = 0.f;
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
	PlayerInputComponent->BindAxis("MoveRight", this, &AmcPawn::MoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AmcPawn::MoveForward);
	PlayerInputComponent->BindAxis("LookPitch", this, &AmcPawn::LookPitch);
	PlayerInputComponent->BindAxis("LookYaw", this, &AmcPawn::LookYaw);
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

void AmcPawn::MoveRight(float AxisValue)
{
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 50.0f;
	CurrentVelocity.Z = 0;
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
			{
				OurCamera->SetRelativeLocation(CurrentVelocity + OurCamera->GetRelativeLocation());
			}
		}
		else
		{
			if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
			{
				OurCamera->SetRelativeLocation(CurrentVelocity + OurCamera->GetRelativeLocation());
			}
		}
	}

}

void AmcPawn::MoveForward(float AxisValue)
{
	CurrentVelocity.Z = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 50.0f;
	CurrentVelocity.X = 0;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
			{
				OurCamera->SetRelativeLocation(CurrentVelocity + OurCamera->GetRelativeLocation());
			}
		}
		else
		{
			if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
			{

				OurCamera->SetRelativeLocation(CurrentVelocity + OurCamera->GetRelativeLocation());
			}
		}
	}
}

void AmcPawn::LookPitch(float AxisValue)
{
	CameraRotation.Yaw = 0;
	CameraRotation.Roll = 0;
	CameraRotation.Pitch = AxisValue;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
			{
				FRotator NewRotation = OurCamera->GetRelativeRotation();			
				NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + CameraRotation.Pitch, 0.0f, 60.0f);
				OurCamera->SetRelativeRotation(NewRotation);
			}
		}
		else
		{
			if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
			{
				FRotator NewRotation = OurCamera->GetRelativeRotation();
				NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + CameraRotation.Pitch, 0.0f, 60.0f);
				OurCamera->SetRelativeRotation(NewRotation);
			}
		}
	}
}

void AmcPawn::LookYaw(float AxisValue)
{
	CameraRotation.Yaw = AxisValue;
	CameraRotation.Pitch = 0;
	CameraRotation.Roll = 0;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
			{
				FRotator NewRotation = OurCamera->GetRelativeRotation();
				NewRotation.Yaw += CameraRotation.Yaw;
				OurCamera->SetRelativeRotation(NewRotation);
			}
		}
		else
		{
			if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
			{
				FRotator NewRotation = OurCamera->GetRelativeRotation();
				NewRotation.Yaw = FMath::Clamp(NewRotation.Yaw + CameraRotation.Yaw, -30.0f, 30.0f);
				OurCamera->SetRelativeRotation(NewRotation);
			}
		}
	}
}
