#pragma once

#include "CoreMinimal.h"
#include "FPSTest/DataTables/CHWeaponsDT.h"

#include "CHWeaponComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSTEST_API UCHWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess="true"))
	FGameplayTag WeaponTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess="true"))
	UDataTable* WeaponsDT = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess="true"))
	FCHWeaponData WeaponData;

public:
	// Sets default values for this component's properties
	UCHWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool GetWeaponDataFromDT(const FGameplayTag& Tag, FCHWeaponData& Data);
	bool ChangeWeaponData(const FGameplayTag& NewWeaponTag);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetWeapon(const FGameplayTag& NewWeaponTag);

	UFUNCTION(BlueprintPure)
	int32 GetMaxBulletsOnWeapon(int32 MaxAttempts = 10);

	UFUNCTION(BlueprintPure)
	int32 GetMaxAmmoOnWeapon(const FGameplayTag& Weapon) const;

	UFUNCTION(BlueprintPure)
	int32 GetAmmoSpentOnShot();

	UFUNCTION(BlueprintPure)
	USkeletalMesh* GetWeaponSK();

	UFUNCTION(BlueprintPure)
	FCHWeaponData GetWeaponData() const { return WeaponData; }

	UFUNCTION(BlueprintPure)
	UAnimMontage* GetReloadAnimation(bool bReloadFromHip) const;
};
