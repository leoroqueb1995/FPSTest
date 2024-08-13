// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CHStaminaInfo.generated.h"

struct FGameplayTag;
class ACHCharacterBase;
class UProgressBar;

UCLASS()
class FPSTEST_API UCHStaminaInfo : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="References", meta=(AllowPrivateAccess="true"))
	ACHCharacterBase* PlayerReference = nullptr;

	FTimerHandle HideWidgetHandle;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Info", meta=(BindWidget))
	UProgressBar* StaminaBar = nullptr;

public:
	UFUNCTION(BlueprintCallable)
	void SetStaminaBarValue(const FGameplayTag& StaminaTag, float NewValue);

	UFUNCTION(BlueprintCallable)
	void InitWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFadeOutAnim();
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerReference(ACHCharacterBase* Player);
};
