// Fill out your copyright notice in the Description page of Project Settings.


#include "CHHUDWidget.h"

#include "CHEndGameWidget.h"
#include "CHPointsWidget.h"
#include "CHStaminaInfo.h"
#include "CHTimerWidget.h"
#include "CHWeaponInfoWidget.h"
#include "Components/Image.h"
#include "FPSTest/Character/CHCharacterBase.h"
#include "FPSTest/Misc/CHFunctionUtils.h"
#include "FPSTest/Misc/CHUtils.h"
#include "FPSTest/PlayerController/CHPlayerController.h"
#include "FPSTest/Weapon/CHWeaponBase.h"

void UCHHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	HideAllElements();
	ShowBasicHUD();
}

void UCHHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCHHUDWidget::ChangeWidgetVisibility(UUserWidget* Widget, ESlateVisibility NewVisibility) const
{
	CHECK_POINTER(Widget)
	Widget->SetVisibility(NewVisibility);
}

void UCHHUDWidget::HideAllElements_Implementation() const
{
	ChangeWidgetVisibility(MatchCooldownWidget);
	ChangeWidgetVisibility(RespawnCooldownWidget);
	ChangeWidgetVisibility(WeaponInfoWidget);
	ChangeWidgetVisibility(EndGameWidget);
	ChangeWidgetVisibility(StaminaInfoWidget);
	ChangeWidgetVisibility(PointsWidget);
	ChangeWidgetVisibility(HealthWidget);
	ChangeWidgetVisibility(TableScoreWidget);
	IMG_Crosshair->SetVisibility(ESlateVisibility::Collapsed);
	IMG_HitMarker->SetVisibility(ESlateVisibility::Collapsed);
}

void UCHHUDWidget::OnGameStart() const
{
	CHECK_POINTER(MatchCooldownWidget)
	ChangeWidgetVisibility(MatchCooldownWidget, ESlateVisibility::Visible);
	MatchCooldownWidget->StartCountdown(true, 1.3f, 500.f);

	FTimerHandle BeginMatch;
	GetWorld()->GetTimerManager().SetTimer(BeginMatch, [&]
	{
		ShowCrosshair(true);
		CHECK_POINTER(GetWorld())
		const ACHPlayerController* PlayerController = UCHFunctionUtils::GetCHPlayerController(GetWorld());
		CHECK_POINTER(PlayerController)
		PlayerController->ReplaceMappingContext(ECHGameState::PLAY, PlayerReference->GetCharacterConfig());
	}, MatchCooldownWidget->GetCountdown(), false);
}

void UCHHUDWidget::OnPlayerDead()
{
	CHECK_POINTER(RespawnCooldownWidget)
	ChangeWidgetVisibility(RespawnCooldownWidget, ESlateVisibility::Visible);
	RespawnCooldownWidget->StartCountdown();
}

void UCHHUDWidget::OnPlayerRespawn()
{
	CHECK_POINTER(RespawnCooldownWidget)
	ChangeWidgetVisibility(RespawnCooldownWidget);
	RespawnCooldownWidget->DestroyTimer();
}

void UCHHUDWidget::OnGameEnd(FText WinnerName)
{
	HideAllElements();
	
	CHECK_POINTER(EndGameWidget)
	EndGameWidget->SetWinnerName(WinnerName);
	ChangeWidgetVisibility(EndGameWidget, ESlateVisibility::Visible);
}

void UCHHUDWidget::InitializeHUD(ACHCharacterBase* Player)
{
	CHECK_POINTER(Player)
	SetPlayerReference(Player);

	PlayerReference->OnCharacterDead.AddDynamic(this, &ThisClass::OnPlayerDead);
	PlayerReference->OnCharacterRespawn.AddDynamic(this, &ThisClass::OnPlayerRespawn);
	
	UpdateWeaponInfo();
	
	CHECK_POINTER(StaminaInfoWidget)
	StaminaInfoWidget->SetPlayerReference(PlayerReference);
	StaminaInfoWidget->InitWidget();

	CHECK_POINTER(PointsWidget)
	PointsWidget->SetPlayerReference(PlayerReference);

	CHECK_POINTER(HealthWidget)
	HealthWidget->SetPlayerReference(PlayerReference);
	HealthWidget->InitWidget();
	
	CHECK_POINTER(PlayerReference->GetPlayerWeapon())
	PlayerReference->GetPlayerWeapon()->OnHit.AddDynamic(this, &ThisClass::SpawnHitMarker);
}

void UCHHUDWidget::SetPlayerReference(ACHCharacterBase* Player)
{
	CHECK_POINTER(Player)
	PlayerReference = Player;
}

void UCHHUDWidget::ShowCrosshair(bool bShow) const
{
	CHECK_POINTER(IMG_Crosshair);
	bShow ? IMG_Crosshair->SetVisibility(ESlateVisibility::Visible) : IMG_Crosshair->SetVisibility(ESlateVisibility::Collapsed);
}

void UCHHUDWidget::ShowBasicHUD()
{
	ShowCrosshair();
	ChangeWidgetVisibility(WeaponInfoWidget, ESlateVisibility::Visible);
	ChangeWidgetVisibility(PointsWidget, ESlateVisibility::Visible);
	ChangeWidgetVisibility(HealthWidget, ESlateVisibility::Visible);
}

void UCHHUDWidget::SpawnHitMarker(bool bKill)
{
	CHECK_POINTER(IMG_HitMarker)
	
	if(bKill)
	{
		IMG_HitMarker->SetColorAndOpacity(FLinearColor::Red);
	}
	else
	{
		IMG_HitMarker->SetColorAndOpacity(FLinearColor::White);
	}
	
	IMG_HitMarker->SetVisibility(ESlateVisibility::Visible);
	FTimerHandle HitMarkerHandle;
	GetWorld()->GetTimerManager().SetTimer(HitMarkerHandle, [&]
	{
		IMG_HitMarker->SetVisibility(ESlateVisibility::Collapsed);
	}, 0.1f, false);
}

void UCHHUDWidget::UpdateWeaponInfo()
{
	CHECK_POINTER(PlayerReference)
	CHECK_POINTER(PlayerReference->GetPlayerWeapon())
	CHECK_POINTER(WeaponInfoWidget)
	
	const ECHWeaponSelected WeaponSelected = UCHFunctionUtils::GetWeaponSelectedEnumFromGameplayTag(PlayerReference->GetPlayerWeapon()->GetWeaponTag());
	const int32 WeaponAmmo = PlayerReference->GetPlayerWeapon()->GetRemainingBullets();
	const int32 PlayerAmmo = PlayerReference->GetCurrentAmmo(PlayerReference->GetPlayerWeapon()->GetWeaponTag());
	
	WeaponInfoWidget->SetWidgetData(PlayerReference, WeaponSelected, WeaponAmmo, PlayerAmmo);
}

void UCHHUDWidget::ShowStamina()
{
	ChangeWidgetVisibility(StaminaInfoWidget, ESlateVisibility::Visible);
}

void UCHHUDWidget::ShowTableScore(bool bShow)
{
	if(bShow)
	{
		ChangeWidgetVisibility(TableScoreWidget, ESlateVisibility::Visible);
		TableScoreWidget->UpdateTableScore();
	}
	else
	{
		ChangeWidgetVisibility(TableScoreWidget);
	}
}

