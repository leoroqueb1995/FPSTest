// Fill out your copyright notice in the Description page of Project Settings.


#include "CHFunctionUtils.h"

#include "CHUtils.h"
#include "FPSTest/CHGameMode.h"
#include "FPSTest/DataAssets/CHDACharacterConfig.h"
#include "FPSTest/Defines/CHGameplayTagDefines.h"
#include "FPSTest/PlayerController/CHPlayerController.h"
#include "Kismet/GameplayStatics.h"

ACHPlayerController* UCHFunctionUtils::GetCHPlayerController(const UObject* WorldContext)
{
	return Cast<ACHPlayerController>(UGameplayStatics::GetPlayerController(WorldContext,0));
}

ACHGameMode* UCHFunctionUtils::GetCHGameMode(const UObject* WorldContext)
{
	return Cast<ACHGameMode>(UGameplayStatics::GetGameMode(WorldContext));
}

FGameplayTag UCHFunctionUtils::GetGameplayTagAssociatedWithWeaponSelected(ECHWeaponSelected WeaponSelected)
{
	switch (WeaponSelected)
	{
	case ECHWeaponSelected::SMG:
		return TAG_WEAPON_SMG;
	case ECHWeaponSelected::PISTOL:
		return TAG_WEAPON_PISTOL;
	case ECHWeaponSelected::SHOTGUN:
		return TAG_WEAPON_SHOTGUN;
	}

	return FGameplayTag();
}

ECHWeaponSelected UCHFunctionUtils::GetWeaponSelectedEnumFromGameplayTag(const FGameplayTag& WeaponSelected)
{
	if(WeaponSelected.MatchesTag(TAG_WEAPON_SMG))
	{
		return ECHWeaponSelected::SMG;
	}
	if(WeaponSelected.MatchesTag(TAG_WEAPON_PISTOL))
	{
		return ECHWeaponSelected::PISTOL;
	}
	if(WeaponSelected.MatchesTag(TAG_WEAPON_SHOTGUN))
	{
		return ECHWeaponSelected::SHOTGUN;
	}
	
	return ECHWeaponSelected::SMG;
}

void UCHFunctionUtils::GetFormattedTime(float TotalSeconds, const FCHTime& Time, int32& Hours, int32& Minutes, int32& Seconds)
{
	if (!Time.IsZero())
	{
		TotalSeconds = Time.GetTotalSeconds();
	}

	const int32 TotalSecondsInt = static_cast<int32>(TotalSeconds);

	Hours = TotalSecondsInt / 3600;
	const int32 RemainingSeconds = TotalSecondsInt % 3600;
	Minutes = RemainingSeconds / 60;
	Seconds = RemainingSeconds % 60;

	if (TotalSeconds - TotalSecondsInt > 0.0f)
	{
		Seconds++;

		if (Seconds >= 60)
		{
			Seconds -= 60;
			Minutes++;
		}

		if (Minutes >= 60)
		{
			Minutes -= 60;
			Hours++;
		}
	}
}
