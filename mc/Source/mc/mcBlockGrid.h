// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "mcBlockGrid.generated.h"

/** Class used to spawn blocks and manage score */
UCLASS(minimalapi)
class AmcBlockGrid : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot{ nullptr };

	/** Text component for the score */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* ScoreText{ nullptr };

	class UParticleSystemComponent* BoomPSC{ nullptr };

	TArray<class AmcBlock*> NewBlocks;
public:
	AmcBlockGrid();

	/** How many blocks have been clicked */
	int32 Score{ 0 };

	/** Number of blocks along each side of grid */
	//UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	//int32 Size;
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	int32 MaxX {0};
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	int32 MaxY {0};

	/** Spacing of blocks */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSpacing{0};
	enum { mc_mine=-1, mc_null=0 };
	TArray<TArray<int32>> Grids;
	TArray<TArray<bool>> Flags;
	int32 MineSize{ 0 };
	float PosX{ 0.f };
	float PosY{0.f};
protected:
	// Begin AActor interface
	virtual void BeginPlay() override;
	// End AActor interface

public:

	/** Handle the block being clicked */
	void AddScore();
	void BlockClicked(int32 x, int32 y);
	void Boom(int32 x, int32 y);

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns ScoreText subobject **/
	FORCEINLINE class UTextRenderComponent* GetScoreText() const { return ScoreText; }
};



