// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "CHRespawnPoint.generated.h"

class UArrowComponent;

UCLASS()
class FPSTEST_API ACHRespawnPoint : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Components, meta=(AllowPrivateAccess="true"))
	UArrowComponent* SpawnReference = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Components, meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* RespawnPointMesh = nullptr;
	
public:
	// Sets default values for this actor's properties
	ACHRespawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FTransform GetSpawnReference() const { return SpawnReference->GetComponentTransform(); }
};
