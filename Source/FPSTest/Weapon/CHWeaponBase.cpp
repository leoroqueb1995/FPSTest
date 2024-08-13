#include "CHWeaponBase.h"

#include "FPSTest/Character/CHCharacterBase.h"
#include "Particles/ParticleSystem.h"
#include "FPSTest/Components/CHWeaponComponent.h"
#include "FPSTest/Misc/CHUtils.h"
#include "FPSTest/Components/CHStatsComponent.h"
#include "FPSTest/Defines/CHGameplayTagDefines.h"
#include "Kismet/GameplayStatics.h"

namespace CHWeaponHelpers
{
	static const FName BulletSocket = TEXT("bullet_socket");
}

// Sets default values
ACHWeaponBase::ACHWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponSK = CreateDefaultSubobject<USkeletalMeshComponent>(FName(TEXT("WeaponSK")));
	SetRootComponent(WeaponSK);

	WeaponComponent = CreateDefaultSubobject<UCHWeaponComponent>(FName(TEXT("WeaponComponent")));
}

// Called when the game starts or when spawned
void ACHWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	InitWeaponData();
}

void ACHWeaponBase::InitWeaponData()
{
	CHECK_POINTER(WeaponComponent)
	CHECK_VALID_TAG(WeaponTag)
	
	WeaponComponent->SetWeapon(WeaponTag);

	CHECK_POINTER(WeaponComponent->GetWeaponSK())
	WeaponSK->SetSkeletalMesh(WeaponComponent->GetWeaponSK());
	
	BulletsLeft = WeaponComponent->GetMaxBulletsOnWeapon();
}

// Called every frame
void ACHWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACHWeaponBase::ApplyDamageToActor_Implementation(AActor* TargetActor)
{
	ICHDamageInterface::ApplyDamageToActor_Implementation(TargetActor);

	CHECK_POINTER(TargetActor)
	CHECK_POINTER(WeaponComponent)

	ACHCharacterBase* CastedActor = Cast<ACHCharacterBase>(TargetActor);
	CHECK_POINTER(CastedActor)
	CHECK_POINTER(CastedActor->GetStatsComponent())

	if (!CastedActor->GetStatsComponent()->HasStat(TAG_STAT_HEALTH))
	{
		return;
	}

	const float BulletDamage = WeaponComponent->GetWeaponData().BulletDamage;
	if (BulletDamage == static_cast<float>(INDEX_NONE))
	{
		return;
	}

	OnHit.Broadcast(false);
	UGameplayStatics::ApplyDamage(CastedActor, BulletDamage, GetInstigatorController(), WeaponOwner, UDamageType::StaticClass());
}

void ACHWeaponBase::SetWeaponBullets(int32 BulletsAmount)
{
	BulletsLeft = FMath::Clamp(BulletsAmount, 0, GetWeaponComponent()->GetMaxBulletsOnWeapon());
}

void ACHWeaponBase::Shoot()
{
	if (GetRemainingBullets() <= 0)
	{
		// TODO: No bullets sound
		return;
	}

	SpawnShootVFX();
	ShootBullet();
	OnWeaponAmmoModified.Broadcast(GetRemainingBullets());
}

void ACHWeaponBase::SpawnShootVFX()
{
	CHECK_POINTER(WeaponComponent)
	CHECK_POINTER(GetWorld())
	CHECK_POINTER(WeaponSK)
	CHECK_POINTER_WITH_TEXT_VOID(WeaponComponent->GetWeaponData().ShootVFX, TEXT("No ShootVFX Data on DT_Weapons"))
	//CHECK_POINTER_WITH_TEXT_VOID(WeaponComponent->GetWeaponData().TracerVFX, TEXT("No TracerVFX data on DT_Weapons"))

	UParticleSystem* VFX = WeaponComponent->GetWeaponData().ShootVFX;
	//UParticleSystem* TracerVFX = WeaponComponent->GetWeaponData().TracerVFX;

	const FVector Location = WeaponSK->GetSocketLocation(CHWeaponHelpers::BulletSocket);
	//const FRotator TracerRotation = WeaponSK->GetSocketRotation(CHWeaponHelpers::BulletSocket);
	//const FVector TracerLocation = Location + (TracerRotation.Vector() * 100.f);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFX, Location, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerVFX, TracerLocation, TracerRotation, true, EPSCPoolMethod::AutoRelease);
}

void ACHWeaponBase::ShootBullet()
{
	if(GetRemainingBullets() == 0)
	{
		return;
	}
	CHECK_POINTER(WeaponOwner)
	CHECK_POINTER(GEngine)
	
	FVector SocketLocation = WeaponSK->GetSocketLocation(CHWeaponHelpers::BulletSocket);
	FRotator SocketRotation = WeaponSK->GetSocketRotation(CHWeaponHelpers::BulletSocket);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	CHECK_POINTER(PlayerController)
	
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	FVector AimDirection = CameraRotation.Vector();
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (AimDirection * 10000.0f); 

	// Make a linetrace from camera to get any hit
	FHitResult CameraHitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); 

	bool bCameraHit = GetWorld()->LineTraceSingleByChannel(CameraHitResult, TraceStart, TraceEnd, ECC_Visibility, Params);
	
	if (bCameraHit)
	{
		TraceEnd = CameraHitResult.Location;
	}
	
	FVector AdjustedDirection = (TraceEnd - SocketLocation).GetSafeNormal();
	FVector AdjustedTraceEnd = SocketLocation + (AdjustedDirection * (bCameraHit ? CameraHitResult.Distance : 10000.0f));

	FHitResult WeaponHitResult;
	bool bWeaponHit = GetWorld()->LineTraceSingleByChannel(WeaponHitResult, SocketLocation, AdjustedTraceEnd, ECC_Visibility, Params);

	// Dibujar la línea de depuración
	FColor LineColor = bWeaponHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), SocketLocation, AdjustedTraceEnd, LineColor, false, 2.0f, 0, 1.0f);

	if (bWeaponHit)
	{
		DrawDebugPoint(GetWorld(), WeaponHitResult.Location, 10.0f, FColor::Yellow, false, 2.0f);
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *WeaponHitResult.GetActor()->GetName());
	}
	
	// One bullet was shot
	int32 BulletsShot = GetWeaponComponent()->GetAmmoSpentOnShot();
	if (BulletsShot == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Weapon data is not valid or there is some missing info."), *FString(__FUNCTION__))
		BulletsShot = 1;
	}
	
	TakeBulletFromMagazine(BulletsShot);
	WeaponOwner->UpdateWeaponAmmo(WeaponTag, BulletsLeft);
	
	if (bWeaponHit)
	{
		CHECK_POINTER(WeaponHitResult.GetActor())
		ACHCharacterBase* Player = Cast<ACHCharacterBase>(WeaponHitResult.GetActor());

		if (!Player)
		{
			return;
		}
		Execute_ApplyDamageToActor(this, Player);

		CHECK_POINTER(WeaponComponent)
		CHECK_POINTER(WeaponComponent->GetWeaponData().HitVFX)

		UParticleSystem* VFX = WeaponComponent->GetWeaponData().HitVFX;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFX, WeaponHitResult.ImpactPoint, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
	}
}

void ACHWeaponBase::TakeBulletFromMagazine(int32 NumberOfBullets)
{
	BulletsLeft -= NumberOfBullets;

	if (BulletsLeft < 0)
	{
		BulletsLeft = 0;
	}
}

bool ACHWeaponBase::Reload(int32 AmmoQuantity, bool bConsumeAmmo)
{
	CHECK_POINTER(GetWeaponComponent(), false)
	CHECK_POINTER(WeaponOwner, false)
	
	if (!GetWeaponComponent()->GetWeaponData().IsValid())
	{
		return false;
	}
	
	const int32 AmmoNeeded = WeaponComponent->GetMaxBulletsOnWeapon() - BulletsLeft;
	const int32 Remain = AmmoQuantity - AmmoNeeded;

	if(AmmoNeeded == 0)
	{
		return true;
	}
	
	if (GetWeaponComponent()->GetWeaponData().ReloadType == ECHReloadType::MAGAZINE)
	{
		if (Remain < 0)
		{
			BulletsLeft += AmmoQuantity;
			if (bConsumeAmmo)
			{
				WeaponOwner->RemoveAmmo(WeaponTag, AmmoQuantity);
			}
		}
		else
		{
			BulletsLeft += AmmoNeeded;
			if (bConsumeAmmo)
			{
				WeaponOwner->RemoveAmmo(WeaponTag, AmmoNeeded);
			}
		}
		WeaponOwner->UpdateWeaponAmmo(WeaponTag, BulletsLeft);
		OnWeaponAmmoModified.Broadcast(GetRemainingBullets());
		OnWeaponReloaded.Broadcast(WeaponOwner->GetCurrentAmmo(WeaponTag));
		return true;
	}

	if (AmmoQuantity < 1)
	{
		return false;
	}
	
	BulletsLeft++;
	if(bConsumeAmmo)
	{
		WeaponOwner->RemoveAmmo(WeaponTag,1);
	}
	else
	{
		// it will only enter on this branch if cheating or after picking up ammo refill
		BulletsLeft = WeaponComponent->GetMaxBulletsOnWeapon();
	}
	WeaponOwner->UpdateWeaponAmmo(WeaponTag, BulletsLeft);
	OnWeaponAmmoModified.Broadcast(GetRemainingBullets());
	OnWeaponReloaded.Broadcast(WeaponOwner->GetCurrentAmmo(WeaponTag));
	return true;
}
