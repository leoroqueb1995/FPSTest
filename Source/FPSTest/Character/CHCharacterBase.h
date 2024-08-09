// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSTest/FPSTestCharacter.h"
#include "FPSTest/Interfaces/CHDamageInterface.h"
#include "CHCharacterBase.generated.h"

struct FGameplayTag;
class ACHWeaponBase;
class UCHStatsComponent;

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
	UInputAction* IA_Aim = nullptr;
	/// ----------- END INPUT ACTIONS ------------ ///

	/// ----------- COMPONENTS & ACTORS --------------- ///
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon, meta=(AllowPrivateAccess="true"))
	UChildActorComponent* WeaponActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon, meta=(AllowPrivateAccess="true"))
	TSubclassOf<ACHWeaponBase> WeaponClass = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UCHStatsComponent* StatsComponent = nullptr;
	/// ----------- END COMPONENTS & ACTORS --------------- ///

	/// ----------- GAMEPLAY VARIABLES -------------- ///
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game|Player", meta=(AllowPrivateAccess="true"))
	int32 Points = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game|Player", meta=(AllowPrivateAccess="true"))
	FText PlayerName;

	bool bIsDead = false;
	/// ----------- END GAMEPLAY VARIABLES -------------- ///
	
	/// --------- ANIM VARIABLES ---------- ///
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
	bool bIsAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
	bool bIsSprinting = false;
	/// --------- END ANIM VARIABLES ---------- ///

public:
	// Sets default values for this character's properties
	ACHCharacterBase();

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

	UFUNCTION()
	void OnStatDepleted(const FGameplayTag& StatDepleted);

	void Dead();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure)
	UCHStatsComponent* GetStatsComponent() const { return StatsComponent; }

	void SetPlayerName();

	virtual void OnDamageTaken_Implementation(AActor* DamageInstigator, float DamageReceived) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void SetIsDead(const bool bDead) { bIsDead = bDead; }

	UFUNCTION(BlueprintPure)
	bool GetIsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure)
	int32 GetPoints() const { return Points; }

	void AddSinglePoint();

	/// --------------- ANIM USEFUL FUNCTIONS -------------- ///
	UFUNCTION(BlueprintPure)
	bool GetIsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintPure)
	bool GetIsSprinting() const { return bIsSprinting; }
};
