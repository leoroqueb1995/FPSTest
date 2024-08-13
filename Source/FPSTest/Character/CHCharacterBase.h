// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FPSTest/FPSTestCharacter.h"
#include "FPSTest/Interfaces/CHDamageInterface.h"
#include "CHCharacterBase.generated.h"

class UCHSprintCameraShake;
class UCHHUDWidget;
enum class ECHWeaponSelected : uint8;
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
	int32 PlayerCurrentAmmo = 0;

	// This will storage actual ammo inside weapons
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponRemainingAmmo = 0;

	FPlayerAmmoData()
	{
	}

	FPlayerAmmoData(const FGameplayTag& Tag, const int32 MaxAmmo) : AmmoTag(Tag), MaxAmmo(MaxAmmo), PlayerCurrentAmmo(MaxAmmo)
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
	UInputAction* IA_TableScore= nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InputActions, meta=(AllowPrivateAccess="true"))
	UInputAction* IA_Respawn = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InputActions, meta=(AllowPrivateAccess="true"))
	UInputAction* IA_Weapon1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InputActions, meta=(AllowPrivateAccess="true"))
	UInputAction* IA_Weapon2 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InputActions, meta=(AllowPrivateAccess="true"))
	UInputAction* IA_Weapon3 = nullptr;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game|Player|Camera", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UCHSprintCameraShake> SprintCameraShakeClass = nullptr;
	
	bool bIsDead = false;
	bool bCanRespawn = false;
	bool bCanShoot = true;
	/// ----------- END GAMEPLAY VARIABLES -------------- ///

	/// ----------- UI ------------ ///
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	UCHHUDWidget* HUD = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UCHHUDWidget> HUDClass = nullptr;

	/// --------- ANIM VARIABLES ---------- ///
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
	bool bIsAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
	bool bIsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
	bool bIsReloading = false;
	/// --------- END ANIM VARIABLES ---------- ///
	
	FTimerHandle ShootTimerHandle;
	FTimerHandle ShootCooldown;

	bool bCameraIsShaking = false;
public:
	// Sets default values for this character's properties
	ACHCharacterBase();

	/// -------- DELEGATES ---------- ///
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FCharacterDead OnCharacterDead;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FCharacterDead OnCharacterRespawn;

private:
	void InitStats();
	void InitWeaponActor();
	
	void ConfigureCamera();

	void CreateHUD();
	
	UFUNCTION()
	void OnMontageNotifyReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void StartRun();
	void ShakeCamera();
	void StopRun();

	void StartAimAction();
	void StartAim();
	void StopAim();
	void StopAimAction();

	void StartShoot();
	void StopShoot();

	void ReloadGun();
	void OnReloadNotify();

	void ChangeToWeaponOne();
	void ChangeToWeaponTwo();
	void ChangeToWeaponThree();
	
	void OnEquipWeaponNotify();

	UFUNCTION(BlueprintNativeEvent)
	void ChangeWeaponActorClass(const ECHWeaponSelected NewWeaponClass);

	UFUNCTION()
	void OnStatDepleted(const FGameplayTag& StatDepleted);

	void PlayRandomHitMontage();
	void Dead();
	UFUNCTION()
	void StartRespawnCooldown();
	void Respawn();

	UFUNCTION(BlueprintCallable)
	void SetIsDead(const bool bDead) { bIsDead = bDead; }

	UFUNCTION(BlueprintCallable)
	void SetCanRespawn(const bool bPlayerCanRespawn) { bCanRespawn = bPlayerCanRespawn; }

	UFUNCTION(BlueprintPure)
	FTransform GetRandomSpawnPoint() const;

	virtual void OnDamageTaken_Implementation(AActor* DamageInstigator, float DamageReceived) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void SetPlayerName();

	UFUNCTION()
	void OnGameStart();

	UFUNCTION()
	void OnGameEnd(ACHCharacterBase* Winner);
	
	void ShowTableScore();
	void HideTableScore();
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure)
	UCHHUDWidget* GetHUD() const { return HUD; }

	UFUNCTION(BlueprintPure)
	UCHStatsComponent* GetStatsComponent() const { return StatsComponent; }

	UFUNCTION(BlueprintPure)
	UCHDACharacterConfig* GetCharacterConfig() const { return CharacterConfig; }

	UFUNCTION(BlueprintPure)
	bool GetIsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure)
	bool PlayerCanRespawn() const { return bCanRespawn; }

	UFUNCTION(BlueprintPure)
	int32 GetPoints() const { return Points; }

	UFUNCTION(BlueprintCallable)
	void AddSinglePoint();

	UFUNCTION(BlueprintPure)
	ACHWeaponBase* GetPlayerWeapon() const;

	void AddAmmo(const FGameplayTag& AmmoType, int32 Amount);
	void RemoveAmmo(const FGameplayTag& AmmoType, int32 Amount = 1);
	int32 GetCurrentAmmo(const FGameplayTag& AmmoType);
	int32 GetWeaponCurrentAmmo(const FGameplayTag& WeaponTag);
	void UpdateWeaponAmmo(const FGameplayTag& WeaponTag, int32 NewValue);

	void FillAmmoInventory();

	UFUNCTION(BlueprintPure)
	FText GetPlayerName() const { return PlayerName; }

	/// --------------- ANIM USEFUL FUNCTIONS -------------- ///
	UFUNCTION(BlueprintPure)
	bool GetIsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintPure)
	bool GetIsSprinting() const { return bIsSprinting; }

	UFUNCTION(BlueprintPure)
	bool GetIsReloading() const { return bIsReloading; }
};
