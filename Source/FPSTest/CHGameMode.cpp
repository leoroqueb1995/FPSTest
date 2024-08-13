// Fill out your copyright notice in the Description page of Project Settings.


#include "CHGameMode.h"

#include "PlayerController/CHPlayerController.h"

ACHGameMode::ACHGameMode()
{
	PlayerControllerClass = ACHPlayerController::StaticClass();
}

void ACHGameMode::BeginPlay()
{
	Super::BeginPlay();

	/**
	FTimerHandle BeginMatch;
	GetWorld()->GetTimerManager().SetTimer(BeginMatch, [this]
	{
		OnGameStart.Broadcast();
	}, 1.f, false);*/
}
