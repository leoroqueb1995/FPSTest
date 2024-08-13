// Fill out your copyright notice in the Description page of Project Settings.


#include "CHStatsComponent.h"

#include "FPSTest/Defines/CHGameplayTagDefines.h"
#include "FPSTest/Misc/CHUtils.h"


// Sets default values for this component's properties
UCHStatsComponent::UCHStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Stats.Reset();
}


// Called when the game starts
void UCHStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UCHStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCHStatsComponent::AddStat(const FStatData& NewStat)
{
	Stats.AddUnique(NewStat);
}

void UCHStatsComponent::RemoveStat(const FGameplayTag& Stat)
{
	for (FStatData StatElem : Stats)
	{
		if (!StatElem.IsEqual(Stat))
		{
			continue;
		}

		Stats.Remove(StatElem);
	}
}

bool UCHStatsComponent::SetBaseValue(const FGameplayTag& Stat, const float NewValue)
{
	const int32 Index = GetStatIndex(Stat);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	FStatData* Data = &Stats[Index];

	if (!Data || !Data->IsValid())
	{
		return false;
	}

	const float ClampedValue = FMath::Clamp(NewValue, 0.f, NewValue);
	Data->ModifyBaseValue(ClampedValue);

	// Update Current value based on BaseValue
	const float CurrentValue = Data->GetBaseValue();
	SetCurrentValue(Stat, CurrentValue);

	return true;
}

bool UCHStatsComponent::SetCurrentValue(const FGameplayTag& Stat, const float NewValue)
{
	const int32 Index = GetStatIndex(Stat);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	FStatData* Data = &Stats[Index];

	if (!Data || !Data->IsValid())
	{
		return false;
	}

	const float ClampedValue = FMath::Clamp(NewValue, 0.f, GetBaseValue(Stat));

	Data->ModifyCurrentValue(ClampedValue);

	if (ClampedValue == 0.f)
	{
		OnStatDepleted.Broadcast(Stat);
	}

	OnStatModified.Broadcast(Stat, ClampedValue);
	return true;
}

bool UCHStatsComponent::IncreaseCurrentValue(const FGameplayTag& Stat, const float ValueToAdd)
{
	CHECK_VALID_TAG(Stat, false)

	const float CurrentValue = GetCurrentValue(Stat);
	if(CurrentValue == static_cast<float>(INDEX_NONE) || CurrentValue >= GetBaseValue(Stat))
	{
		return false;
	}

	const float AddedValue = CurrentValue + ValueToAdd;
	return SetCurrentValue(Stat, AddedValue);
}

bool UCHStatsComponent::GetStatData(const FGameplayTag& Stat, FStatData& Data)
{
	CHECK_VALID_TAG(Stat, false)

	for (FStatData& StatItem : Stats)
	{
		if (StatItem.IsEqual(Stat))
		{
			Data = StatItem;
			return true;
		}
	}

	return false;
}

float UCHStatsComponent::GetCurrentValue(const FGameplayTag& Stat)
{
	FStatData Data;
	if (!GetStatData(Stat, Data))
	{
		return INDEX_NONE;
	}

	return Data.GetCurrentValue();
}

float UCHStatsComponent::GetBaseValue(const FGameplayTag& Stat)
{
	FStatData Data;
	if (!GetStatData(Stat, Data))
	{
		return INDEX_NONE;
	}

	return Data.GetBaseValue();
}

int32 UCHStatsComponent::GetStatIndex(const FGameplayTag& Stat)
{
	if (!Stat.IsValid())
	{
		return INDEX_NONE;
	}

	for (int32 i = 0; i < Stats.Num(); i++)
	{
		if (Stats[i].IsEqual(Stat))
		{
			return i;
		}
	}

	return INDEX_NONE;
}

bool UCHStatsComponent::HasStat(const FGameplayTag& Stat)
{
	for (FStatData StatElem : Stats)
	{
		if (!StatElem.IsValid() || !StatElem.IsEqual(Stat))
		{
			continue;
		}

		return true;
	}

	return false;
}

void UCHStatsComponent::ResetStats()
{
	for (int32 i = 0; i < Stats.Num(); i++)
	{
		SetCurrentValue(Stats[i].StatTag, Stats[i].GetBaseValue());
	}
}

bool UCHStatsComponent::ConsumeStamina()
{
	CHECK_POINTER(GetWorld(), false)
	if (!HasStat(TAG_STAT_STAMINA) || GetCurrentValue(TAG_STAT_STAMINA) <= 0.f)
	{
		return false;
	}

	// Clean Regeneration if any
	if (GetWorld()->GetTimerManager().TimerExists(RegenerateStaminaTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(RegenerateStaminaTimerHandle);
	}

	// Start consuming
	if (!GetWorld()->GetTimerManager().TimerExists(ConsumeStaminaTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(ConsumeStaminaTimerHandle, [&]
		{
			const float DecreasedStamina = GetCurrentValue(TAG_STAT_STAMINA) - 2.f;
			SetCurrentValue(TAG_STAT_STAMINA, DecreasedStamina);
		}, .1f, true);
	}

	return true;
}

void UCHStatsComponent::StopConsumingStamina()
{
	CHECK_POINTER(GetWorld())
	if (!GetWorld()->GetTimerManager().TimerExists(ConsumeStaminaTimerHandle))
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(ConsumeStaminaTimerHandle);
	RegenerateStamina();
}

void UCHStatsComponent::RegenerateStamina()
{
	CHECK_POINTER(GetWorld())
	if (GetWorld()->GetTimerManager().TimerExists(RegenerateStaminaTimerHandle))
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(RegenerateStaminaTimerHandle, [&]
	{
		const float RecoveredStamina = GetCurrentValue(TAG_STAT_STAMINA) + 7.f;
		SetCurrentValue(TAG_STAT_STAMINA, RecoveredStamina);
		if(RecoveredStamina >= GetBaseValue(TAG_STAT_STAMINA))
		{
			GetWorld()->GetTimerManager().ClearTimer(RegenerateStaminaTimerHandle);
		}
	}, .1f, true, 1.f);
}
