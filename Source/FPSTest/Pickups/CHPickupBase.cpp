// Fill out your copyright notice in the Description page of Project Settings.


#include "CHPickupBase.h"

#include "Components/SphereComponent.h"
#include "FPSTest/Character/CHCharacterBase.h"
#include "FPSTest/Misc/CHUtils.h"


// Sets default values
ACHPickupBase::ACHPickupBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("PickupMesh")));
	InteractionArea = CreateDefaultSubobject<USphereComponent>(FName(TEXT("InteractionArea")));

	GetPickupMesh()->SetCollisionResponseToAllChannels(ECR_Overlap);

	SetRootComponent(PickupMesh);
	InteractionArea->SetupAttachment(RootComponent);

	InteractionArea->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
}

// Called when the game starts or when spawned
void ACHPickupBase::BeginPlay()
{
	Super::BeginPlay();
	StartTime = GetGameTimeSinceCreation();
	InitialPosition = GetActorLocation();
}

void ACHPickupBase::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CHECK_POINTER(OtherActor)

	ACHCharacterBase* Player = Cast<ACHCharacterBase>(OtherActor);
	CHECK_POINTER(Player)

	OnPickup(Player);
}

// Called every frame
void ACHPickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bCanBePicked)
	{
		StartTime += DeltaTime * UpAndDownSpeed;
		const float NewZ = FMath::Sin(StartTime) * UpAndDownDistance;
		
		FVector NewLocation = InitialPosition;
		NewLocation.Z += NewZ;
		
		SetActorLocation(NewLocation);
	}
}

