// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "CHDACharacterConfig.generated.h"

struct FGameplayTag;
enum class ECHGameState : uint8;
class UInputMappingContext;
struct FStatData;

UCLASS()
class FPSTEST_API UCHDACharacterConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	TArray<FStatData> PlayerStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game")
	TArray<FString> PlayerNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game")
	TMap<FGameplayTag, int32> MaxAmmoAmountPerGun;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(ClampMin = 0, UIMin = 0, ClampMax = 2000))
	float JogSpeed = 400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(ClampMin = 0, UIMin = 0, ClampMax = 4000))
	float SprintSpeed = 1200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dead", meta=(ClampMin = 0, UIMin = 0))
	float RespawnCooldownTime = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", meta=(ClampMin = 0, UIMin = 0))
	float NormalCameraDistance = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", meta=(ClampMin = 0, UIMin = 0))
	float ZoomCameraDistance = 50;

	// This will work only on non-NPC players
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameState")
	TMap<ECHGameState, UInputMappingContext*> MappingContexts;

	UFUNCTION(BlueprintPure)
	FText GetRandomName();

	UFUNCTION(BlueprintPure)
	int32 GetMaxAmmoAmount(const FGameplayTag& WeaponTag);
};
