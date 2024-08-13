// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CHWeaponInfoWidget.generated.h"

class UImage;
enum class ECHWeaponSelected : uint8;
class UTextBlock;
class ACHCharacterBase;

UCLASS()
class FPSTEST_API UCHWeaponInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Info", meta=(BindWidget))
	UImage* IMG_Weapon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Info", meta=(BindWidget))
	UTextBlock* TB_WeaponAmmo = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Info", meta=(BindWidget))
	UTextBlock* TB_PlayerAmmo = nullptr;

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetData(ACHCharacterBase* PlayerReference, ECHWeaponSelected WeaponSelected, int32 WeaponAmmo, int32 PlayerAmmo);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetWeaponImage(ECHWeaponSelected SelectedWeapon);
	
	UFUNCTION(BlueprintCallable)
	void SetWeaponAmmo(int32 UpdatedAmmo);

	UFUNCTION(BlueprintCallable)
	void SetPlayerAmmo(int32 UpdatedAmmo);

	UFUNCTION(BlueprintCallable)
	void BindDelegates(ACHCharacterBase* PlayerReference);
};
