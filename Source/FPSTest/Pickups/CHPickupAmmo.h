// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CHPickupBase.h"
#include "CHPickupAmmo.generated.h"

UCLASS()
class FPSTEST_API ACHPickupAmmo : public ACHPickupBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACHPickupAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
