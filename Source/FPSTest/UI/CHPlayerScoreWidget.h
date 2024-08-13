// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSTest/Character/CHCharacterBase.h"
#include "FPSTest/Misc/CHUtils.h"
#include "CHPlayerScoreWidget.generated.h"


class UTextBlock;
class ACHCharacterBase;

UCLASS()
class FPSTEST_API UCHPlayerScoreWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="References", meta=(AllowPrivateAccess="true"))
	ACHCharacterBase* PlayerReference = nullptr;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TB_PlayerName = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TB_Score = nullptr;

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerScore();
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerReference(ACHCharacterBase* Player){ CHECK_POINTER(Player); PlayerReference = Player;}
};
