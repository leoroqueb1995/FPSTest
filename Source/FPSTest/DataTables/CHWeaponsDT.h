

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FPSTest/FPSTest.h"

#include "CHWeaponsDT.generated.h"

USTRUCT(BlueprintType)
struct FCHWeaponData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponData")
	FGameplayTag WeaponTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponData")
	USkeletalMesh* WeaponMesh = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponData")
	EFireRate WeaponFireRate = EFireRate::MEDIUM;

	// This will determine if weapon is reloaded one bullet each time or a complete magazine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponData")
	EReloadType ReloadType = EReloadType::MAGAZINE;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	UParticleSystem* ShootVFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	UParticleSystem* TracerVFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	UParticleSystem* HitVFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game|Shoot")
	int32 BulletsPerMagazine = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game|Shoot")
	int32 PlayerMaxAmount = 1;

	// Maximum bullets that will be shot at once (useful for shotguns)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game|Shoot")
	int32 MaxBulletsPerShot = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	TSubclassOf<AActor> BulletClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	float BulletDamage = 10.f;
	
	bool IsValid() const
	{
		return WeaponTag.IsValid();
	}
};