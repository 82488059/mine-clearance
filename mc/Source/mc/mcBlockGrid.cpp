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
	MaxX = 5;
	MaxY = 6;
	//Size = 5;
	MineSize = MaxX;

	BlockSpacing = 300.f;
	Grids.Reset(MaxX);
	Grids.AddDefaulted(MaxX);
	Flags.Reset(MaxX);
	Flags.AddDefaulted(MaxX);
	// int32 d = Size;
	for (size_t x = 0; x < MaxX; x++)
	{
		Grids[x].Reset(MaxY);
		Grids[x].AddDefaulted(MaxY);
		Flags[x].Reset(MaxY);
		Flags[x].AddDefaulted(MaxY);
		for (size_t y = 0; y < MaxY; y++)
		{
			Grids[x][y] = 0;
			Flags[x][y] = false;
		}
		int32 j = FMath::RandRange(0, MaxY - 1 );
		Grids[x][j] = int32(mc_mine);
	}
	for (size_t x = 0; x < MaxX; x++)
	{
		for (size_t y = 0; y < MaxY; y++)
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
						if (x + i >= MaxX)
						{
							continue;
						}
						if (y + j < 0)
						{
							continue;
						}
						if (y + j >= MaxY)
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
	const int32 NumBlocks = MaxX * MaxY;
	NewBlocks.Reset(NumBlocks);
	// Loop to spawn each block
	PosX = -BlockSpacing * MaxX / 2;
	PosY = -BlockSpacing * MaxY / 2;

	for(int32 BlockIndex=0; BlockIndex < NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex/ MaxY) * BlockSpacing + PosX; // Divide by dimension
		const float YOffset = (BlockIndex% MaxY) * BlockSpacing + PosY; // Modulo gives remainder

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		AmcBlock* NewBlock = GetWorld()->SpawnActor<AmcBlock>(BlockLocation, FRotator(0,0,0));
		// NewBlock->SetActorLocation(BlockLocation);
		NewBlocks.Push(NewBlock);
		// Tell the block about its owner
		if (NewBlock != nullptr)
		{
			NewBlock->SetIndex((BlockIndex / MaxY), (BlockIndex % MaxY));
			NewBlock->SetMine(Grids[(BlockIndex / MaxY)][(BlockIndex % MaxY)]);
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
	const float XOffset = PosX + (x) * BlockSpacing; // Divide by dimension
	const float YOffset = PosY + (y) * BlockSpacing; // Modulo gives remainder

	int32 mine = Grids[x][y];
	if (-1 == mine)
	{
		// 爆炸特效
		if (BoomPSC && BoomPSC->Template)
		{
			BoomPSC->SetRelativeLocation(FVector(XOffset,YOffset, 100.f));
			BoomPSC->ToggleActive();
		}
		// game over
		FString GameOver{ TEXT("Game Over")};
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, GameOver);
		}
	}
	else
	{
		// 打开空白
		Flags[x][y] = true;
		if (0 == Grids[x][y])
		{// 周围没雷，打开周围格子
			TArray<int32> ArrayX;
			TArray<int32> ArrayY;
			ArrayX.Push(x);
			ArrayY.Push(y);
			int32 NowX{x}, NowY{y};
			while (ArrayX.Num() > 0)
			{// 周围没雷，打开周围格子
				NowX = ArrayX.Last();
				NowY = ArrayY.Last();
				// u
				if (NowX + 1 < MaxX)
				{
					if (!Flags[NowX+1][NowY])
					{
						Flags[NowX + 1][NowY] = true;
						NewBlocks[(NowX + 1) * MaxY + NowY]->HandleOpen();
						if (0 == Grids[NowX+1][NowY])
						{
							NowX += 1;
							ArrayX.Push(NowX);
							ArrayY.Push(NowY);
							continue;
						}
					}
				}
				// d
				if (NowX - 1 >= 0)
				{
					if (!Flags[NowX - 1][NowY])
					{
						Flags[NowX - 1][NowY] = true;
						NewBlocks[(NowX - 1) * MaxY + NowY]->HandleOpen();
						if (0 == Grids[NowX - 1][NowY])
						{
							NowX -= 1;
							ArrayX.Push(NowX);
							ArrayY.Push(NowY);
							continue;
						}
					}
				}
				// l
				if (NowY - 1 >= 0)
				{
					if (!Flags[NowX][NowY-1])
					{
						Flags[NowX][NowY-1] = true;
						NewBlocks[NowX * MaxY + NowY - 1]->HandleOpen();
						if (0 == Grids[NowX][NowY-1])
						{
							NowY -= 1;
							ArrayX.Push(NowX);
							ArrayY.Push(NowY);
							continue;
						}
					}
				}
				// r
				if (NowY + 1 < MaxY)
				{
					if (!Flags[NowX][NowY + 1])
					{
						Flags[NowX][NowY + 1] = true;
						NewBlocks[NowX * MaxY + NowY + 1]->HandleOpen();
						if (0 == Grids[NowX][NowY + 1])
						{
							NowY += 1;
							ArrayX.Push(NowX);
							ArrayY.Push(NowY);
							continue;
						}
					}
				}
				ArrayX.Pop();
				ArrayY.Pop();
			}
		}
		// 判断游戏是否胜利 
		int32 find{ 0 };
		int32 notOpen{ 0 };
		for (size_t i = 0; i < Flags.Num(); i++)
		{
			for (size_t j = 0; j < Flags[i].Num(); j++)
			{
				if (!Flags[i][j])
				{
					++notOpen;
					if (-1 == Grids[i][j])
					{
						find += 1;
					}
				}
			}
		}
		if (notOpen == find)
		{
			TArray<FStringFormatArg> FormatArray;
			FormatArray.Add(FStringFormatArg(find));
			FString name = FString::Format(TEXT("You Win Find {0}"), FormatArray);
			// UE_LOG(LogTemp, Warning, TEXT("%s"), *name);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, name);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
