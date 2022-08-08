// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Blueprint/UserWidget.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "mcGameMode.generated.h"

/** GameMode class to specify pawn and playercontroller */
UCLASS(minimalapi)
class AmcGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	/** 游戏开始时，用作菜单的控件类。*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> StartingWidgetClass;
	/** 用作菜单的控件实例。*/
	UPROPERTY()
	UUserWidget* CurrentWidget {nullptr};

public:
	AmcGameMode();
	virtual void BeginPlay() override;
	/** 移除当前菜单控件，并在指定类（如有）中新建控件。*/
	UFUNCTION(BlueprintCallable, Category = "UMG Game")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);


};



