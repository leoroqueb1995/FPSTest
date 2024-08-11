// Fill out your copyright notice in the Description page of Project Settings.


#include "CHPickupHealth.h"

#include "FPSTest/Character/CHCharacterBase.h"
#include "FPSTest/Components/CHStatsComponent.h"
#include "FPSTest/Defines/CHGameplayTagDefines.h"
#include "FPSTest/Misc/CHUtils.h"


// Sets default values
ACHPickupHealth::ACHPickupHealth()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACHPickupHealth::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ACHPickupHealth::OnPickup(ACHCharacterBase* Player)
{
	Super::OnPickup(Player);

	CHECK_POINTER(Player, false)
	CHECK_POINTER(Player->GetStatsComponent(), false)

	if(!Player->GetStatsComponent()->IncreaseCurrentValue(TAG_STAT_HEALTH, 40.f))
	{
		return false;
	}
	
	return true;
}

// Called every frame
void ACHPickupHealth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

