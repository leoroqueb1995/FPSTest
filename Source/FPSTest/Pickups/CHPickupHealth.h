// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CHPickupBase.h"
#include "CHPickupHealth.generated.h"

UCLASS()
class FPSTEST_API ACHPickupHealth : public ACHPickupBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACHPickupHealth();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool OnPickup(ACHCharacterBase* Player) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
