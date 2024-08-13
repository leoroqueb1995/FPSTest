// Fill out your copyright notice in the Description page of Project Settings.


#include "CHPlayerHealthWidget.h"

#include "Components/ProgressBar.h"
#include "FPSTest/Components/CHStatsComponent.h"
#include "FPSTest/Defines/CHGameplayTagDefines.h"

void UCHPlayerHealthWidget::InitWidget()
{
	CHECK_POINTER(PlayerReference)
	CHECK_POINTER(PlayerReference->GetStatsComponent())
	PlayerReference->GetStatsComponent()->OnStatModified.AddDynamic(this, &ThisClass::SetHealthBarValue);
}

void UCHPlayerHealthWidget::SetHealthBarValue(const FGameplayTag& Tag, float NewValue)
{
	if(!Tag.MatchesTag(TAG_STAT_HEALTH))
	{
		return;
	}

	CHECK_POINTER(PB_Health)
	const float BarValue = FMath::Clamp(NewValue/100, 0.f, 1.f);
	PB_Health->SetPercent(BarValue);
}
