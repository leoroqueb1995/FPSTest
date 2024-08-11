// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FPSTest/FPSTestCharacter.h"
#include "FPSTest/Interfaces/CHDamageInterface.h"
#include "CHCharacterBase.generated.h"

class UCHDACharacterConfig;
struct FGameplayTag;
class ACHWeaponBase;
class UCHStatsComponent;

USTRUCT(BlueprintType)
struct FPlayerAmmoData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AmmoTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentAmmo = 0;

	FPlayerAmmoData()
	{
	}

	FPlayerAmmoData(const FGameplayTag& Tag, const int32 MaxAmmo) : AmmoTag(Tag), MaxAmmo(MaxAmmo)
	{
	}

	bool IsEqual(const FGameplayTag& OtherTag) const
	{
		return OtherTag.MatchesTag(AmmoTag);
	}

	bool operator==(const FPlayerAmmoData& Other) const
	{
		return AmmoTag == Other.AmmoTag &&
			MaxAmmo == Other.MaxAmmo;
	}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterDead);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterReload, UAnimationAsset*, ReloadAnim);

UCLASS()
class FPSTEST_API ACHCharacterBase : public AFPSTestCharacter, public ICHDamageInterface
{
	GENERATED_BODY()
	/// ----------- INPUT ACTIONS ------------ ///
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InputActions, meta=(AllowPrivateAccess="true"))
	UInputAction* IA_Run = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InputActions, meta=(AllowPrivateAccess="true"))
	UInputAction* IA_Shoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InputActions, meta=(AllowPrivateAccess="true"))
	UInputAction* IA_Reload = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InputActions, meta=(AllowPrivateAccess="true"))
	UInputAction* IA_Aim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InputActions, meta=(AllowPrivateAccess="true"))
	UInputAction* IA_Respawn = nullptr;
	/// ----------- END INPUT ACTIONS ------------ ///

	/// ----------- COMPONENTS, DATA & ACTORS --------------- ///
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponActor", meta=(AllowPrivateAccess="true"))
	UChildActorComponent* WeaponActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponActor", meta=(AllowPrivateAccess="true"))
	TSubclassOf<ACHWeaponBase> WeaponClass = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UCHStatsComponent* StatsComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game|Player", meta=(AllowPrivateAccess="true"))
	UCHDACharacterConfig* CharacterConfig = nullptr;
	/// ----------- END COMPONENTS, DATA & ACTORS --------------- ///

	/// ----------- GAMEPLAY VARIABLES -------------- ///
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game|Player", meta=(AllowPrivateAccess="true"))
	int32 Points = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game|Player", meta=(AllowPrivateAccess="true"))
	FText PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game|Player", meta=(AllowPrivateAccess="true"))
	TArray<FPlayerAmmoData> AmmoInventory;

	bool bIsDead = false;
	bool bCanRespawn = false;
	/// ----------- END GAMEPLAY VARIABLES -------------- ///

	/// --------- ANIM VARIABLES ---------- ///
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
	bool bIsAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
	bool bIsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
	bool bIsReloading = false;
	/// --------- END ANIM VARIABLES ---------- ///

public:
	// Sets default values for this character's properties
	ACHCharacterBase();

	/// -------- DELEGATES ---------- ///
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FCharacterDead OnCharacterDead;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FCharacterReload OnCharacterReload;

private:
	void InitStats();
	void InitWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void StartRun();
	void StopRun();

	void StartAimAction();
	void StartAim();
	void StopAim();
	void StopAimAction();

	void StartShoot();
	void StopShoot();

	void ReloadGun();
	void ReloadGunDelegate();

	UFUNCTION()
	void OnStatDepleted(const FGameplayTag& StatDepleted);

	void Dead();
	UFUNCTION()
	void StartRespawnCooldown();
	void Respawn();

	UFUNCTION(BlueprintCallable)
	void SetIsDead(const bool bDead) { bIsDead = bDead; }

	UFUNCTION(BlueprintCallable)
	void SetCanRespawn(const bool bPlayerCanRespawn) { bCanRespawn = bPlayerCanRespawn; }

	virtual void OnDamageTaken_Implementation(AActor* DamageInstigator, float DamageReceived) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void SetPlayerName();

	UFUNCTION(BlueprintPure)
	FTransform GetRandomSpawnPoint() const;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure)
	UCHStatsComponent* GetStatsComponent() const { return StatsComponent; }

	UFUNCTION(BlueprintPure)
	bool GetIsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure)
	bool PlayerCanRespawn() const { return bCanRespawn; }

	UFUNCTION(BlueprintPure)
	int32 GetPoints() const { return Points; }

	void AddSinglePoint();

	UFUNCTION(BlueprintPure)
	ACHWeaponBase* GetPlayerWeapon() const;

	void AddAmmo(const FGameplayTag& AmmoType, int32 Amount);
	void RemoveAmmo(const FGameplayTag& AmmoType, int32 Amount = 1);
	int32 GetCurrentAmmo(const FGameplayTag& AmmoType);

	void FillAmmoInventory();

	/// --------------- ANIM USEFUL FUNCTIONS -------------- ///
	UFUNCTION(BlueprintPure)
	bool GetIsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintPure)
	bool GetIsSprinting() const { return bIsSprinting; }

	UFUNCTION(BlueprintPure)
	bool GetIsReloading() const { return bIsReloading; }
};