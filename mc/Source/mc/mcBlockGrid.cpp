// Copyright Epic Games, Inc. All Rights Reserved.

#include "mcBlockGrid.h"
#include "mcBlock.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

AmcBlockGrid::AmcBlockGrid()
{
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
	ScoreText->SetRelativeLocation(FVector(200.f,0.f,0.f));
	ScoreText->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(0)));
	ScoreText->SetupAttachment(DummyRoot);

	// 创建可激活或停止的粒子系统
	BoomPSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BoomParticles"));
	BoomPSC->SetupAttachment(DummyRoot);
	BoomPSC->bAutoActivate = false;
	BoomPSC->SetRelativeLocation(FVector(0.f, 0.0f, 0.0f));
	BoomPSC->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (ParticleAsset.Succeeded())
	{
		BoomPSC->SetTemplate(ParticleAsset.Object);
	}
	// 创建可激活或停止的粒子系统

	// Set defaults
	Size = 5;
	MineSize = Size;
	BlockSpacing = 300.f;
	Grids.Reset(Size);
	Grids.AddDefaulted(Size);
	// int32 d = Size;
	for (size_t x = 0; x < Size; x++)
	{
		Grids[x].Reset(Size);
		Grids[x].AddDefaulted(Size);
		for (size_t y = 0; y < Size; y++)
		{
			Grids[x][y] = 0;
		}
		int32 j = FMath::RandRange(0, Size-1);
		Grids[x][j] = int32(mc_mine);
	}

	for (size_t x = 0; x < Size; x++)
	{
		for (size_t y = 0; y < Size; y++)
		{
			if (Grids[x][y] == mc_mine)
			{
				continue;
			}
			else
			{
				int32 count{ 0 };
				for (int32 i = -1 ; i <= 1; ++i)
				{
					for (int32 j = -1; j <= 1; j++)
					{
						if (x+i < 0)
						{
							continue;
						}
						if (x + i >= Size)
						{
							continue;
						}
						if (y + j < 0)
						{
							continue;
						}
						if (y + j >= Size)
						{
							continue;
						}
						if (mc_mine == Grids[x+i][y+j])
						{
							++count;
						}
					}
				}
				Grids[x][y] = count;
			}
		}
	}

}


void AmcBlockGrid::BeginPlay()
{
	Super::BeginPlay();

	// Number of blocks
	const int32 NumBlocks = Size * Size;
	NewBlocks.Reset(NumBlocks);
	// Loop to spawn each block
	for(int32 BlockIndex=0; BlockIndex<NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex/Size) * BlockSpacing; // Divide by dimension
		const float YOffset = (BlockIndex%Size) * BlockSpacing; // Modulo gives remainder

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		AmcBlock* NewBlock = GetWorld()->SpawnActor<AmcBlock>(BlockLocation, FRotator(0,0,0));
		// NewBlock->SetActorLocation(BlockLocation);
		NewBlocks.Push(NewBlock);
		// Tell the block about its owner
		if (NewBlock != nullptr)
		{
			NewBlock->SetIndex((BlockIndex / Size), (BlockIndex % Size));
			NewBlock->SetMine(Grids[(BlockIndex / Size)][(BlockIndex % Size)]);
			NewBlock->OwningGrid = this;
		}
	}
}


void AmcBlockGrid::AddScore()
{
	// Increment score
	
	// Update text
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(Score)));
}

void AmcBlockGrid::BlockClicked(int32 x, int32 y)
{
	if (mc_mine == Grids[x][y]) {
		// game over;
	}
	else
	{
		// int32 d = Size;
	}
}

void AmcBlockGrid::Boom(int32 x, int32 y)
{
	const float XOffset = (x) * BlockSpacing; // Divide by dimension
	const float YOffset = (y) * BlockSpacing; // Modulo gives remainder

	TArray<FStringFormatArg> FormatArray;
	FormatArray.Add(FStringFormatArg(XOffset));
	FormatArray.Add(FStringFormatArg(YOffset));
	FString name = FString::Format(TEXT("{0},{1}"), FormatArray);
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *name);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, name);
	}
	if (BoomPSC && BoomPSC->Template)
	{
		BoomPSC->SetRelativeLocation(FVector(-XOffset, -YOffset, 100.f));
		BoomPSC->ToggleActive();
	}
}

#undef LOCTEXT_NAMESPACE
