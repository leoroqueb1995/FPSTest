// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/StackBox.h"
#include "CHTableScore.generated.h"

class UCHPlayerScoreWidget;

UCLASS()
class FPSTEST_API UCHTableScore : public UUserWidget
{
	GENERATED_BODY()

	FTimerHandle UpdateTableScoreHandle;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UStackBox* SB_TableScore = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	TSubclassOf<UCHPlayerScoreWidget> PlayerScoreClass = nullptr;

protected:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION()
	void UpdateTableScore();
};
