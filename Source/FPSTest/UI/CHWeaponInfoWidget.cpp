// Fill out your copyright notice in the Description page of Project Settings.


#include "CHWeaponInfoWidget.h"
#include "FPSTest/Character/CHCharacterBase.h"
#include "Components/TextBlock.h"
#include "FPSTest/Misc/CHUtils.h"
#include "FPSTest/Weapon/CHWeaponBase.h"

void UCHWeaponInfoWidget::SetWidgetData(ACHCharacterBase* PlayerReference, ECHWeaponSelected WeaponSelected, int32 WeaponAmmo, int32 PlayerAmmo)
{
	BindDelegates(PlayerReference);
	SetWeaponImage(WeaponSelected);
	SetPlayerAmmo(PlayerAmmo);
	SetWeaponAmmo(WeaponAmmo);
}

void UCHWeaponInfoWidget::SetWeaponAmmo(int32 UpdatedAmmo)
{
	CHECK_POINTER(TB_WeaponAmmo)
	TB_WeaponAmmo->SetText(FText::FromString(FString::FromInt(UpdatedAmmo)));
}

void UCHWeaponInfoWidget::SetPlayerAmmo(int32 UpdatedAmmo)
{
	CHECK_POINTER(TB_PlayerAmmo)
	TB_PlayerAmmo->SetText(FText::FromString(FString::FromInt(UpdatedAmmo)));
}

void UCHWeaponInfoWidget::BindDelegates(ACHCharacterBase* PlayerReference)
{
	CHECK_POINTER(PlayerReference)
	CHECK_POINTER(PlayerReference->GetPlayerWeapon())
	
	if(!PlayerReference->GetPlayerWeapon()->OnWeaponReloaded.IsBound())
	{
		PlayerReference->GetPlayerWeapon()->OnWeaponReloaded.AddDynamic(this, &ThisClass::SetPlayerAmmo);
	}
	
	if(!PlayerReference->GetPlayerWeapon()->OnWeaponAmmoModified.IsBound())
	{
		PlayerReference->GetPlayerWeapon()->OnWeaponAmmoModified.AddDynamic(this, &ThisClass::SetWeaponAmmo);
	}
	
}
