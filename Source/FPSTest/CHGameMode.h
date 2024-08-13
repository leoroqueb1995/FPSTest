// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CHGameMode.generated.h"

class ACHCharacterBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameEnd, ACHCharacterBase*, Winner);
UCLASS()
class FPSTEST_API ACHGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACHGameMode();
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGameStart OnGameStart;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGameEnd OnGameEnd;

protected:
	virtual void BeginPlay() override;
	
};
