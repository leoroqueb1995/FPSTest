// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FPSTest.generated.h"

#define FAST_FIRE_RATE 0.12f;
#define MEDIUM_FIRE_RATE 0.2f;
#define SLOW_FIRE_RATE 2.f;

UENUM(BlueprintType)
enum class ECHFireRate : uint8
{
	SLOW	UMETA(DisplayName = "SlowFireRate"),
	MEDIUM	UMETA(DisplayName = "MediumFireRate"),
	FAST	UMETA(DisplayName = "FastFireRate"),
};

UENUM(BlueprintType)
enum class ECHReloadType : uint8
{
	SINGLE_BULLET		UMETA(DisplayName = "SingleBullet"),
	MAGAZINE	UMETA(DisplayName = "CompleteMagazine")
};

UENUM(BlueprintType)
enum class ECHFireMode : uint8
{
	SINGLE_SHOT		UMETA(DisplayName = "Single Shot"),
	AUTOMATIC		UMETA(DisplayName = "Automatic")
};

UENUM(BlueprintType)
enum class ECHGameState : uint8
{
	PLAY		UMETA(DisplayName = "Play"),
	DEAD		UMETA(DisplayName = "Dead"),
	MENU		UMETA(DisplayName = "Menu")
};

UENUM(BlueprintType)
enum class ECHWeaponSelected : uint8
{
	SMG		UMETA(DisplayName = "SMG"),
	PISTOL		UMETA(DisplayName = "Pistol"),
	SHOTGUN		UMETA(DisplayName = "Shotgun"),
};


USTRUCT(BlueprintType)
struct FCHTime
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Hours"))
	int32 Hours = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Minutes"))
	int32 Minutes = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Seconds"))
	int32 Seconds = 0;

	float GetTotalSeconds() const
	{
		return Hours * 3600.0f + Minutes * 60.0f + Seconds;
	}

	bool IsZero() const
	{
		return Hours <= 0.0f && Minutes <= 0.0f && Seconds <= 0.0f;
	}
	
	FCHTime(){}
	
	static FCHTime FromSeconds(const float Seconds)
	{
		FCHTime TimeFromSeconds;

		if (Seconds > 0)
		{
			TimeFromSeconds.Minutes = Seconds / 60;
			TimeFromSeconds.Seconds = Seconds - TimeFromSeconds.Minutes * 60;
			TimeFromSeconds.Hours = TimeFromSeconds.Minutes / 60;
			TimeFromSeconds.Minutes = TimeFromSeconds.Minutes - TimeFromSeconds.Hours * 60;
		}
		else
		{
			TimeFromSeconds.Minutes = 0;
		}
		
		return TimeFromSeconds;
	}

	static FCHTime FromTimespan(const FTimespan& Timespan)
	{
		FCHTime TimeFromTimespan;
		
		TimeFromTimespan.Hours = Timespan.GetDays() * 24;
		TimeFromTimespan.Hours += Timespan.GetHours();
		TimeFromTimespan.Minutes = Timespan.GetMinutes();
		TimeFromTimespan.Seconds = Timespan.GetSeconds();
		
		return TimeFromTimespan;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("Hours: %i Minutes: %i Seconds: %i"), Hours, Minutes, Seconds);
	}
};