// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHPickupBase.generated.h"

class ACHCharacterBase;
class USphereComponent;

UCLASS()
class FPSTEST_API ACHPickupBase : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* PickupMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta=(AllowPrivateAccess="true"))
	USphereComponent* InteractionArea = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game|Visuals", meta=(AllowPrivateAccess="true"))
	float UpAndDownSpeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game|Visuals", meta=(AllowPrivateAccess="true"))
	float UpAndDownDistance = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Game", meta=(AllowPrivateAccess="true"))
	float CooldownTime = 30.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Game", meta=(AllowPrivateAccess="true"))
	bool bCanBePicked = true;

	// Up&Down movement
	float StartTime = 0.f;
	FVector InitialPosition;

public:
	// Sets default values for this actor's properties
	ACHPickupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnPickup(ACHCharacterBase* Player){}
	
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UStaticMeshComponent* GetPickupMesh() const { return PickupMesh; }
	float GetCooldownTime() const { return CooldownTime; }
	bool GetCanBePicked() const { return bCanBePicked; }
	void SetCanBePicked(const bool bNewValue) { bCanBePicked = bNewValue; }

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
