// Fill out your copyright notice in the Description page of Project Settings.


#include "CHPickupAmmo.h"

#include "FPSTest/Character/CHCharacterBase.h"
#include "FPSTest/Misc/CHUtils.h"


// Sets default values
ACHPickupAmmo::ACHPickupAmmo()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACHPickupAmmo::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ACHPickupAmmo::OnPickup(ACHCharacterBase* Player)
{
	Super::OnPickup(Player);

	CHECK_POINTER(Player, false)
	Player->FillAmmoInventory();
	return true;
}

// Called every frame
void ACHPickupAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

