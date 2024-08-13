// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CHPlayerHealthWidget.h"
#include "CHTableScore.h"
#include "Blueprint/UserWidget.h"
#include "CHHUDWidget.generated.h"

class UCHPointsWidget;
class UCHStaminaInfo;
class UCHEndGameWidget;
class UImage;
class UCHWeaponInfoWidget;
class ACHCharacterBase;
class UCHTimerWidget;

UCLASS()
class FPSTEST_API UCHHUDWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="References", meta=(AllowPrivateAccess="true"))
	ACHCharacterBase* PlayerReference = nullptr;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown", meta=(BindWidget))
	UCHTimerWidget* MatchCooldownWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown", meta=(BindWidget))
	UCHTimerWidget* RespawnCooldownWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(BindWidget))
	UCHWeaponInfoWidget* WeaponInfoWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(BindWidget))
	UCHEndGameWidget* EndGameWidget = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player", meta=(BindWidget))
	UCHStaminaInfo* StaminaInfoWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player", meta=(BindWidget))
	UCHPointsWidget* PointsWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player", meta=(BindWidget))
	UCHPlayerHealthWidget* HealthWidget = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD", meta=(BindWidget))
	UCHTableScore* TableScoreWidget = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD", meta=(BindWidget))
	UImage* IMG_Crosshair;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD", meta=(BindWidget))
	UImage* IMG_HitMarker;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void ChangeWidgetVisibility(UUserWidget* Widget, ESlateVisibility NewVisibility = ESlateVisibility::Collapsed) const;

	UFUNCTION(BlueprintNativeEvent)
	void HideAllElements() const;

	void SetPlayerReference(ACHCharacterBase* Player);

	void ShowCrosshair(bool bShow = true) const;

	void ShowBasicHUD();
	
public:
	UFUNCTION(BlueprintCallable)
	void OnGameStart() const;

	UFUNCTION(BlueprintCallable)
	void OnPlayerDead();

	UFUNCTION(BlueprintCallable)
	void OnPlayerRespawn();

	UFUNCTION(BlueprintCallable)
	void OnGameEnd(FText WinnerName);

	UFUNCTION(BlueprintCallable)
	void InitializeHUD(ACHCharacterBase* Player);

	void UpdateWeaponInfo();

	void ShowStamina();

	void ShowTableScore(bool bShow);

	UFUNCTION()
	void SpawnHitMarker(bool bKill = false);
};
