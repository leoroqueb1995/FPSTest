// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "CHStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatModified, const FGameplayTag&, Stat, float, UpdatedValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStatDepleted, const FGameplayTag&, Stat);

USTRUCT(BlueprintType)
struct FStatData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag StatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseValue = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentValue = 1.f;

	FStatData(){}

	FStatData(const FGameplayTag& StatTag, const float BaseValue) :
		StatTag(StatTag), BaseValue(BaseValue), CurrentValue(BaseValue){}

	void ModifyBaseValue(const float NewBaseValue)
	{
		BaseValue = NewBaseValue;
	}

	void ModifyCurrentValue(const float NewCurrentValue)
	{
		CurrentValue = NewCurrentValue;
	}
	
	const FGameplayTag& GetStatID() const
	{
		return StatTag;
	}

	float GetCurrentValue() const
	{
		return CurrentValue;
	}
	
	float GetBaseValue() const
	{
		return BaseValue;
	}

	bool IsEqual(const FGameplayTag& Tag) const
	{
		return Tag.MatchesTag(StatTag);
	}

	bool IsValid() const
	{
		return StatTag.IsValid();
	}

	bool operator==(const FStatData& Other) const
	{
		return GetStatID().MatchesTag(Other.GetStatID())
				&& GetBaseValue() == Other.GetBaseValue()
				&& GetCurrentValue() == Other.GetCurrentValue();
	}
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSTEST_API UCHStatsComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats, meta=(AllowPrivateAccess="true"))
	TArray<FStatData> Stats;

	/// ----------- STAMINA ------------ ///
	FTimerHandle ConsumeStaminaTimerHandle;
	FTimerHandle RegenerateStaminaTimerHandle;

public:
	// Sets default values for this component's properties
	UCHStatsComponent();

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FStatDepleted OnStatDepleted;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FStatModified OnStatModified;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void AddStat(const FStatData& NewStat);
	
	UFUNCTION(BlueprintCallable)
	void RemoveStat(const FGameplayTag& Stat);

	UFUNCTION(BlueprintCallable)
	bool SetBaseValue(const FGameplayTag& Stat, const float NewValue);

	UFUNCTION(BlueprintCallable)
	bool SetCurrentValue(const FGameplayTag& Stat, const float NewValue);
	
	UFUNCTION(BlueprintCallable)
	bool IncreaseCurrentValue(const FGameplayTag& Stat, const float ValueToAdd);

	UFUNCTION(BlueprintPure)
	bool GetStatData(const FGameplayTag& Stat, FStatData& Data);

	UFUNCTION(BlueprintPure)
	float GetCurrentValue(const FGameplayTag& Stat);

	UFUNCTION(BlueprintPure)
	float GetBaseValue(const FGameplayTag& Stat);

	UFUNCTION(BlueprintPure)
	int32 GetStatIndex(const FGameplayTag& Stat);

	UFUNCTION(BlueprintPure)
	bool HasStat(const FGameplayTag& Stat);

	// This will set CurrentValue and BuffValue to 0 on each stat (useful for saving stats on savegame)
	UFUNCTION(BlueprintCallable)
	void ResetStats();

	/// --------- STAMINA ------------ ///
	bool ConsumeStamina();
	void StopConsumingStamina();
	void RegenerateStamina();
	
	TArray<FStatData> GetStats() {return Stats;}
	void SetStats(TArray<FStatData> NewStats){ Stats = NewStats; }
};
