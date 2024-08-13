// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CHPointsWidget.generated.h"

class UTextBlock;
class ACHCharacterBase;

UCLASS()
class FPSTEST_API UCHPointsWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="References", meta=(AllowPrivateAccess="true"))
	ACHCharacterBase* PlayerReference = nullptr;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_Kills = nullptr;

public:

	void SetPlayerReference(ACHCharacterBase* Player){ PlayerReference = Player; }
};
