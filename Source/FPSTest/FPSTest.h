// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EFireRate : uint8
{
	SLOW	UMETA(DisplayName = "SlowFireRate"),
	MEDIUM	UMETA(DisplayName = "MediumFireRate"),
	FAST	UMETA(DisplayName = "FastFireRate"),
};

UENUM(BlueprintType)
enum class EReloadType : uint8
{
	SINGLE		UMETA(DisplayName = "SingleBullet"),
	MAGAZINE	UMETA(DisplayName = "CompleteMagazine")
};