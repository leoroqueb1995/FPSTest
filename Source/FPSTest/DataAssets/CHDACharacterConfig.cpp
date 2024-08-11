// Fill out your copyright notice in the Description page of Project Settings.


#include "CHDACharacterConfig.h"

#include "FPSTest/Misc/CHUtils.h"

FText UCHDACharacterConfig::GetRandomName()
{
	CHECK_EMPTY_ARRAY(PlayerNames, FText())
	const int32 RandomIndex = FMath::RandRange(0, PlayerNames.Num()-1);
	const FString RandomName = PlayerNames[RandomIndex];
	
	return FText::FromString(RandomName);
}

int32 UCHDACharacterConfig::GetMaxAmmoAmount(const FGameplayTag& WeaponTag)
{
	const int32* AmmoAmount = MaxAmmoAmountPerGun.Find(WeaponTag);

	if(!AmmoAmount)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: No data for tag {%s}"), *FString(__FUNCTION__), *WeaponTag.ToString());
		return INDEX_NONE;
	}

	return *AmmoAmount;
}
