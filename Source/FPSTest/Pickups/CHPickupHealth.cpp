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

void ACHPickupHealth::OnPickup(ACHCharacterBase* Player)
{
	Super::OnPickup(Player);

	CHECK_POINTER(Player)
	CHECK_POINTER(Player->GetStatsComponent())

	if(!Player->GetStatsComponent()->IncreaseCurrentValue(TAG_STAT_HEALTH, 40.f))
	{
		return;
	}
	
	SetCanBePicked(false);
	// TODO: Change visuals
	FTimerHandle CooldownTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, [&]
	{
		SetCanBePicked(true);
	}, GetCooldownTime(), false);
}

// Called every frame
void ACHPickupHealth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

