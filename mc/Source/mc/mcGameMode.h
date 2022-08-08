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
	/** ��Ϸ��ʼʱ�������˵��Ŀؼ��ࡣ*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> StartingWidgetClass;
	/** �����˵��Ŀؼ�ʵ����*/
	UPROPERTY()
	UUserWidget* CurrentWidget {nullptr};

public:
	AmcGameMode();
	virtual void BeginPlay() override;
	/** �Ƴ���ǰ�˵��ؼ�������ָ���ࣨ���У����½��ؼ���*/
	UFUNCTION(BlueprintCallable, Category = "UMG Game")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);


};



