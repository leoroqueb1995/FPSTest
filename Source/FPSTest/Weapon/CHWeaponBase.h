#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FPSTest/Interfaces/CHDamageInterface.h"

#include "CHWeaponBase.generated.h"

class ACHCharacterBase;
class UCHWeaponComponent;

UCLASS()
class FPSTEST_API ACHWeaponBase : public AActor, public ICHDamageInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess="true"))
	FGameplayTag WeaponTag;

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void InitWeaponData();
	
	void SpawnShootVFX();
	void RaycastBullet();
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	UCHWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }
	
	virtual void ApplyDamageToActor_Implementation(AActor* TargetActor) override;

	UFUNCTION(BlueprintPure)
	int32 GetRemainingBullets() const { return BulletsLeft; }

	void Shoot();

	void SetWeaponOwner(ACHCharacterBase* NewOwner) { WeaponOwner = NewOwner; }
};
