// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CHFunctionUtils.generated.h"

class ACHPlayerController;

UCLASS()
class FPSTEST_API UCHFunctionUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static ACHPlayerController* GetCHPlayerController(const UObject* WorldContext);
};
