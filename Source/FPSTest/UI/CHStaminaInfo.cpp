// Fill out your copyright notice in the Description page of Project Settings.


#include "CHStaminaInfo.h"

#include "Components/ProgressBar.h"
#include "FPSTest/Character/CHCharacterBase.h"
#include "FPSTest/Components/CHStatsComponent.h"
#include "FPSTest/Defines/CHGameplayTagDefines.h"
#include "FPSTest/Misc/CHUtils.h"

void UCHStaminaInfo::SetStaminaBarValue(const FGameplayTag& StaminaTag, float NewValue)
{
	if(!StaminaTag.MatchesTag(TAG_STAT_STAMINA))
	{
		return;
	}

	CHECK_POINTER(StaminaBar)
	CHECK_POINTER(GetWorld())
	
	const float BarValue = FMath::Clamp(NewValue/100, 0.f, 1.f);
	StaminaBar->SetPercent(BarValue);

	if(GetWorld()->GetTimerManager().TimerExists(HideWidgetHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(HideWidgetHandle);
	}
	
	GetWorld()->GetTimerManager().SetTimer(HideWidgetHandle, [this]
	{
		PlayFadeOutAnim();
	}, 1.1f, false);
}

void UCHStaminaInfo::InitWidget()
{
	CHECK_POINTER(PlayerReference)
	CHECK_POINTER(PlayerReference->GetStatsComponent())
	
	PlayerReference->GetStatsComponent()->OnStatModified.AddDynamic(this, &ThisClass::SetStaminaBarValue);
}

void UCHStaminaInfo::SetPlayerReference(ACHCharacterBase* Player)
{
	CHECK_POINTER(Player)
	PlayerReference = Player;
}
