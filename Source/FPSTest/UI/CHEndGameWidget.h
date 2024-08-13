// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CHEndGameWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class FPSTEST_API UCHEndGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Info", meta=(BindWidget))
	UTextBlock* TB_WinnerName = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Buttons", meta=(BindWidget))
	UButton* BT_ExitGame = nullptr;

protected:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetWinnerName(FText Name);

	UFUNCTION()
	void OnExitButtonClicked();
};
