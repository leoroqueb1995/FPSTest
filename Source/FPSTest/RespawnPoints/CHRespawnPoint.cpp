// Fill out your copyright notice in the Description page of Project Settings.


#include "CHRespawnPoint.h"

#include "Components/ArrowComponent.h"


// Sets default values
ACHRespawnPoint::ACHRespawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RespawnPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("RespawnPointMesh")));
	SpawnReference = CreateDefaultSubobject<UArrowComponent>(FName(TEXT("SpawnReference")));

	SpawnReference->SetupAttachment(RespawnPointMesh);
}

// Called when the game starts or when spawned
void ACHRespawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACHRespawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

