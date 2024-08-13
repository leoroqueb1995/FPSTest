// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSTest/FPSTest.h"
#include "CHTimerWidget.generated.h"

struct FCHTime;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCountdownCompleted);
UCLASS()
class FPSTEST_API UCHTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_Time = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NoTimeText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CountdownSeconds = 5.0f;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FCountdownCompleted OnCountDownCompleted;
	
protected:
	
	bool bCountdown = false;
	bool bShouldDestroy = false;
	float FontSize = 0.f;
	float DestroySeconds = 0.f;
	
	float AccumulativeTime = 0.f;
	float OriginalCountdown = 0.f;
public:
	UFUNCTION(BlueprintCallable, meta=(Time="(Hours=0,Minutes=0,Seconds=0)"))
	virtual void UpdateStaticTime(float TotalSeconds, FCHTime Time = FCHTime(), bool bShowFormat = false);

	UFUNCTION(BlueprintCallable, meta=(Time="(Hours=0,Minutes=0,Seconds=0)"))
	virtual void UpdateCounterTime(float TotalSeconds, FCHTime Time = FCHTime(), bool bShowFormat = false);

	UFUNCTION(BlueprintCallable)
	virtual void SetNoTime();

	UFUNCTION(BlueprintCallable)
	void StartCountdown(bool bDestroyOnCompleted = false, const float DestroyDelaySeconds = 0, const float FontSizeOverride = 0, const float OverrideCountdown = 0);

	UFUNCTION(BlueprintCallable)
	void OverrideTextFontSize(const float NewSize);

	UFUNCTION(BlueprintCallable)
	void DestroyTimer(const float InitialDelay = 0);

	UFUNCTION(BlueprintPure)
	float GetCountdown() const { return OriginalCountdown; }

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
};
