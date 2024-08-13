#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FPSTest/Defines/CHGameplayTagDefines.h"
#include "FPSTest/Interfaces/CHDamageInterface.h"

#include "CHWeaponBase.generated.h"

class ACHCharacterBase;
class UCHWeaponComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponUsed, int32, UpdatedAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShotHit, bool, bKilled);
UCLASS()
class FPSTEST_API ACHWeaponBase : public AActor, public ICHDamageInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess="true"))
	FGameplayTag WeaponTag = TAG_WEAPON_SMG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon, meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* WeaponSK = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon, meta=(AllowPrivateAccess="true"))
	UCHWeaponComponent* WeaponComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess="true"))
	UParticleSystem* MuzzleVFX = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess="true"))
	int32 BulletsLeft = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess="true"))
	ACHCharacterBase* WeaponOwner = nullptr;

public:
	// Sets default values for this actor's properties
	ACHWeaponBase();

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FWeaponUsed OnWeaponAmmoModified;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FWeaponUsed OnWeaponReloaded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FShotHit OnHit;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void InitWeaponData();

	void SpawnShootVFX();
	void ShootBullet();
	void TakeBulletFromMagazine(int32 NumberOfBullets = 1);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	UCHWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

	virtual void ApplyDamageToActor_Implementation(AActor* TargetActor) override;

	UFUNCTION(BlueprintPure)
	int32 GetRemainingBullets() const { return BulletsLeft; }

	void SetWeaponBullets(int32 BulletsAmount);

	void Shoot();

	// Will return ammo exceed if any
	bool Reload(int32 AmmoQuantity, bool bConsumeAmmo = true);

	UFUNCTION(BlueprintCallable)
	void SetWeaponOwner(ACHCharacterBase* NewOwner) { WeaponOwner = NewOwner; }

	UFUNCTION(BlueprintPure)
	const FGameplayTag& GetWeaponTag() const { return WeaponTag; }
};
