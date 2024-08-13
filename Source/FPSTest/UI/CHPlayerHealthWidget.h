// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSTest/Character/CHCharacterBase.h"
#include "FPSTest/Misc/CHUtils.h"
#include "CHPlayerHealthWidget.generated.h"

class ACHCharacterBase;
class UProgressBar;

UCLASS()
class FPSTEST_API UCHPlayerHealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="References", meta=(AllowPrivateAccess="true"))
	ACHCharacterBase* PlayerReference = nullptr;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Info", meta=(BindWidget))
	UProgressBar* PB_Health = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetPlayerReference(ACHCharacterBase* Player){ CHECK_POINTER(Player); PlayerReference = Player;}

	UFUNCTION(BlueprintCallable)
	void InitWidget();

	UFUNCTION(BlueprintCallable)
	void SetHealthBarValue(const FGameplayTag& Tag, float NewValue);
	
};
