// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CHFunctionUtils.generated.h"

class ACHGameMode;
struct FCHTime;
enum class ECHWeaponSelected : uint8;
struct FGameplayTag;
class ACHPlayerController;

UCLASS()
class FPSTEST_API UCHFunctionUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static ACHPlayerController* GetCHPlayerController(const UObject* WorldContext);

	UFUNCTION(BlueprintPure)
	static ACHGameMode* GetCHGameMode(const UObject* WorldContext);

	UFUNCTION(BlueprintPure)
	static FGameplayTag GetGameplayTagAssociatedWithWeaponSelected(ECHWeaponSelected WeaponSelected);

	UFUNCTION(BlueprintPure)
	static ECHWeaponSelected GetWeaponSelectedEnumFromGameplayTag(const FGameplayTag& WeaponSelected);
	
	UFUNCTION(BlueprintCallable)
	static void GetFormattedTime(float TotalSeconds, const FCHTime& Time, int32& Hours, int32& Minutes, int32& Seconds);
};
