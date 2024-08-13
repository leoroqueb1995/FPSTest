// Fill out your copyright notice in the Description page of Project Settings.


#include "CHTimerWidget.h"

#include "Components/TextBlock.h"
#include "FPSTest/FPSTest.h"
#include "FPSTest/Misc/CHFunctionUtils.h"
#include "FPSTest/Misc/CHUtils.h"

void UCHTimerWidget::UpdateStaticTime(float TotalSeconds, FCHTime Time, bool bShowFormat)
{
	int32 Hours;
	int32 Minutes;
	int32 Seconds;
	UCHFunctionUtils::GetFormattedTime(TotalSeconds, Time, Hours, Minutes, Seconds);

	FString TimeString = "";
	int32 LittleMask = Hours + Minutes + Seconds;

	bool bMarkInit = false;
	
	if ((Hours > 0 || bMarkInit) && LittleMask)
	{
		TimeString += FString(" ") + FString::FromInt(Hours);
		if(bShowFormat)
		{
			TimeString += FString("h");
		}
		LittleMask -= Hours;
		bMarkInit = true;
	}

	if ((Minutes > 0 || bMarkInit) && LittleMask)
	{
		TimeString += FString(" ") + FString::FromInt(Minutes);
		if(bShowFormat)
		{
			TimeString += FString("m");
		}
		LittleMask -= Minutes;
	}

	if ((Seconds > 0) && LittleMask)
	{
		TimeString += FString(" ") + FString::FromInt(Seconds);
		if(bShowFormat)
		{
			TimeString += FString("s");
		}
	}
	
	TB_Time->SetText(FText::FromString(TimeString));
}

void UCHTimerWidget::UpdateCounterTime(float TotalSeconds, FCHTime Time, bool bShowFormat)
{
	int32 Hours = 0;
	int32 Minutes = 0;
	int32 Seconds = 0;
	UCHFunctionUtils::GetFormattedTime(TotalSeconds, Time, Hours, Minutes, Seconds);

	bool bStringStarted = false;
	FString TimeString = "";

	if (Hours > 0)
	{
		TimeString += FString::FromInt(Hours);
		
		if(bShowFormat)
		{
			TimeString += FString("h");
		}
		
		bStringStarted = true;
	}

	if ((Minutes > 0 || bStringStarted))
	{
		TimeString += FString(" ") + FString::FromInt(Minutes);
		if(bShowFormat)
		{
			TimeString += FString("m");
		}
		bStringStarted = true;
	}

	if ((Seconds >= 0 || bStringStarted))
	{
		if (Hours == 0 && Minutes == 0 && Seconds == 0)
		{
			Seconds++;
		}
		
		TimeString += FString(" ") + FString::FromInt(Seconds);
		if(bShowFormat)
		{
			TimeString += FString("s");
		}
	}
	
	TB_Time->SetText(FText::FromString(TimeString));
}

void UCHTimerWidget::SetNoTime()
{
	TB_Time->SetText(NoTimeText);
}

void UCHTimerWidget::StartCountdown(bool bDestroyOnCompleted, const float DestroyDelaySeconds, const float FontSizeOverride, const float OverrideCountdown)
{
	if(OverrideCountdown > 0)
	{
		CountdownSeconds = OverrideCountdown;
		OriginalCountdown = OverrideCountdown;
	}
	UpdateStaticTime(CountdownSeconds);
	FontSize = FontSizeOverride;
	OverrideTextFontSize(FontSize);
	DestroySeconds = DestroyDelaySeconds;
	bShouldDestroy = bDestroyOnCompleted;
	bCountdown = true;
}

void UCHTimerWidget::OverrideTextFontSize(const float NewSize)
{
	FSlateFontInfo Font = TB_Time->GetFont();
	Font.Size = NewSize;
	
	TB_Time->SetFont(Font);
}

void UCHTimerWidget::DestroyTimer(const float InitialDelay)
{
	if(InitialDelay == 0)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	CHECK_POINTER(GetWorld())
	
	FTimerHandle DelayHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this]
	{
		DestroyTimer(); // Recursive call with 0 delay
	}, InitialDelay, false);
}

void UCHTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(bCountdown)
	{
		AccumulativeTime += InDeltaTime;
		if (AccumulativeTime >= 1.0f)
		{
			CountdownSeconds = CountdownSeconds - 1.0f;
			UpdateCounterTime(CountdownSeconds);
			AccumulativeTime -= 1.0f;
			
			if(CountdownSeconds == 0)
			{
				SetNoTime();
				OnCountDownCompleted.Broadcast();
				if(bShouldDestroy)
				{
					DestroyTimer(DestroySeconds);
				}
				
				// Reset timer countdown
				AccumulativeTime = 0.f;
				CountdownSeconds = OriginalCountdown;

				bCountdown = false;
			}
		}
	}
	
}

void UCHTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OriginalCountdown = CountdownSeconds;
	UpdateStaticTime(CountdownSeconds);
}
