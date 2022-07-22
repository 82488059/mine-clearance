// Copyright Epic Games, Inc. All Rights Reserved.


#include "mcBlock.h"
#include "mcBlockGrid.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"


#define LOCTEXT_NAMESPACE "PuzzleBlock"

AmcBlock::AmcBlock()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RedMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			, RedMaterial(TEXT("/Game/Puzzle/Meshes/RedMaterial.RedMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;
	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(1.f,1.f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,25.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	BlockMesh->OnClicked.AddDynamic(this, &AmcBlock::BlockClicked);
	BlockMesh->OnInputTouchBegin.AddDynamic(this, &AmcBlock::OnFingerPressedBlock);
	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	RedMaterial = ConstructorStatics.RedMaterial.Get();
	// number 
	Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text0"));
	Text->SetRelativeLocation(FVector(-130.f, -70.f, 90.f));
	Text->SetRelativeRotation(FRotator(90.f, 180.f, 0.f));
	Text->SetRelativeScale3D(FVector(8.f, 8.f, 8.f));
	Text->SetText(FText::Format(LOCTEXT("ScoreFmt", "{0}"), FText::AsNumber(0)));
	Text->SetupAttachment(DummyRoot);
	Text->SetHiddenInGame(true);
	// 

}

void AmcBlock::BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked)
{
	HandleClicked();
}

void AmcBlock::BlockRightClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked)
{
	HandleRightClicked();
}


void AmcBlock::OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	HandleClicked();
}

void AmcBlock::OnFingerPressedBlockRight(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	HandleRightClicked();
}

void AmcBlock::HandleClicked()
{
	// Check we are not already active
	if (!bIsActive)
	{
		bIsActive = true;
		// Change material
		if (-1 == Mine)
		{
			// boom
			BlockMesh->SetMaterial(0, RedMaterial);
			if (OwningGrid != nullptr)
			{
				OwningGrid->AddScore();
			}
		}
		else if (0 != Mine)
		{
			BlockMesh->SetMaterial(0, OrangeMaterial);
			Text->SetHiddenInGame(false);
		}
		else
		{
			BlockMesh->SetMaterial(0, OrangeMaterial);
		}
	}
}

void AmcBlock::HandleRightClicked()
{
	if (bIsActive)
	{
	}
}

void AmcBlock::Highlight(bool bOn)
{
	// Do not highlight if the block has already been activated.
	if (bIsActive)
	{
		return;
	}
	if (bOn)
	{
		BlockMesh->SetMaterial(0, BaseMaterial);
	}
	else
	{
		BlockMesh->SetMaterial(0, BlueMaterial);
	}
}

void AmcBlock::SetIndex(int32 x, int32 y)
{
	GridX = x;
	GridY = y;
}

void AmcBlock::GetIndex(int32& x, int32& y)
{
	x = GridX;
	y = GridY;
}

int32 AmcBlock::GetIndexX()
{
	return GridX;
}

int32 AmcBlock::GetIndexY()
{
	return GridY;
}

void AmcBlock::SetMine(int32 size)
{
	Mine = size;
	Text->SetText(FText::Format(LOCTEXT("ScoreFmt", "{0}"), FText::AsNumber(Mine)));
}

int32 AmcBlock::GetMine()
{
	return Mine;
}


#undef LOCTEXT_NAMESPACE
