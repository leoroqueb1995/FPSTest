#include "CHWeaponComponent.h"

#include "FPSTest/Misc/CHUtils.h"


// Sets default values for this component's properties
UCHWeaponComponent::UCHWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UCHWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWeaponDataFromDT(WeaponTag, WeaponData);
}

bool UCHWeaponComponent::GetWeaponDataFromDT(const FGameplayTag& Tag, FCHWeaponData& Data)
{
	CHECK_POINTER(WeaponsDT, false)
	CHECK_VALID_TAG(WeaponTag, false)

	TArray<FCHWeaponData*> Rows;
	WeaponsDT->GetAllRows<FCHWeaponData>(TEXT(""), Rows);

	CHECK_EMPTY_ARRAY(Rows, false);
	for (const FCHWeaponData* Row : Rows)
	{
		if (!Row->IsValid())
		{
			continue;
		}

		if (Row->WeaponTag.MatchesTag(WeaponTag))
		{
			Data = *Row;
			return true;
		}
	}

	return false;
}

bool UCHWeaponComponent::ChangeWeaponData(const FGameplayTag& NewWeaponTag)
{
	CHECK_VALID_TAG(NewWeaponTag, false)
	
	WeaponTag = NewWeaponTag;
	return GetWeaponDataFromDT(WeaponTag, WeaponData);
}


// Called every frame
void UCHWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCHWeaponComponent::SetWeapon(const FGameplayTag& NewWeaponTag)
{
	CHECK_VALID_TAG(NewWeaponTag)
	ChangeWeaponData(NewWeaponTag);
}

int32 UCHWeaponComponent::GetMaxBulletsOnWeapon(int32 MaxAttempts)
{
	// Try to retrieve WeaponData from DT
	if (!WeaponData.IsValid())
	{
		CHECK_VALID_TAG(WeaponTag, INDEX_NONE)
		GetWeaponDataFromDT(WeaponTag, WeaponData);

		if (MaxAttempts == 0)
		{
			return INDEX_NONE;
		}

		GetMaxBulletsOnWeapon(MaxAttempts - 1);
	}

	return WeaponData.BulletsPerMagazine;
}

int32 UCHWeaponComponent::GetMaxAmmoOnWeapon(const FGameplayTag& Weapon) const
{
	CHECK_POINTER(WeaponsDT, false)
	
	TArray<FCHWeaponData*> Rows;
	WeaponsDT->GetAllRows<FCHWeaponData>(TEXT(""), Rows);
	
	CHECK_EMPTY_ARRAY(Rows, false);
	for (const FCHWeaponData* Row : Rows)
	{
		if (!Row->IsValid())
		{
			continue;
		}

		if (Row->WeaponTag.MatchesTag(Weapon))
		{
			return Row->BulletsPerMagazine;
		}
	}
	
	return INDEX_NONE;
}

int32 UCHWeaponComponent::GetAmmoSpentOnShot()
{
	if(!GetWeaponData().IsValid())
	{
		return INDEX_NONE;
	}
	return GetWeaponData().MaxBulletsPerShot;
}

USkeletalMesh* UCHWeaponComponent::GetWeaponSK()
{
	if (!WeaponData.IsValid())
	{
		return nullptr;
	}

	return WeaponData.WeaponMesh;
}

UAnimMontage* UCHWeaponComponent::GetReloadAnimation(bool bReloadFromHip) const
{
	if (!WeaponData.IsValid())
	{
		return nullptr;
	}
	
	UAnimMontage* ReloadAnim;
	bReloadFromHip ? ReloadAnim = WeaponData.HipReloadAnimation : ReloadAnim = WeaponData.AimReloadAnimation;
	
	return ReloadAnim;
}