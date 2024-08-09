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
	
	if(!CastedActor->GetStatsComponent()->HasStat(TAG_STAT_HEALTH))
	{
		return;
	}

	const float BulletDamage = WeaponComponent->GetWeaponData().BulletDamage;
	if( BulletDamage == static_cast<float>(INDEX_NONE))
	{
		return;
	}

	UGameplayStatics::ApplyDamage(CastedActor, BulletDamage, GetInstigatorController(), WeaponOwner, UDamageType::StaticClass());
}

void ACHWeaponBase::Shoot()
{
	if(GetRemainingBullets() <= 0)
	{
		// TODO: No bullets sound
		return;
	}
	
	SpawnShootVFX();
	RaycastBullet();
}

void ACHWeaponBase::SpawnShootVFX()
{
	CHECK_POINTER(WeaponComponent)
	CHECK_POINTER(GetWorld())
	CHECK_POINTER(WeaponSK)
	CHECK_POINTER_WITH_TEXT_VOID(WeaponComponent->GetWeaponData().ShootVFX, TEXT("No ShootVFX Data on DT_Weapons"))
	CHECK_POINTER_WITH_TEXT_VOID(WeaponComponent->GetWeaponData().TracerVFX, TEXT("No TracerVFX data on DT_Weapons"))

	UParticleSystem* VFX = WeaponComponent->GetWeaponData().ShootVFX;
	//UParticleSystem* TracerVFX = WeaponComponent->GetWeaponData().TracerVFX;
	
	const FVector Location = WeaponSK->GetSocketLocation(CHWeaponHelpers::BulletSocket);
	//const FRotator TracerRotation = WeaponSK->GetSocketRotation(CHWeaponHelpers::BulletSocket);
	//const FVector TracerLocation = Location + (TracerRotation.Vector() * 100.f);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFX, Location, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerVFX, TracerLocation, TracerRotation, true, EPSCPoolMethod::AutoRelease);
}

void ACHWeaponBase::RaycastBullet()
{
	FVector SocketLocation = WeaponSK->GetSocketLocation(CHWeaponHelpers::BulletSocket);
	FRotator SocketRotation = WeaponSK->GetSocketRotation(CHWeaponHelpers::BulletSocket);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	CHECK_POINTER(PlayerController)

	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	const FVector2D ScreenCenter(ViewportSizeX * .5f, ViewportSizeY * .5f); // More efficient to * 0.5 than /2

	FVector WorldLocation, WorldDirection;
	PlayerController->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection);

	FVector EndLocation = WorldLocation + (WorldDirection * 10000.f);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// Make bullet raycast
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		SocketLocation,
		EndLocation,
		ECC_Pawn,
		Params
	);

	// TODO: Change this to hit any surface
	if (bHit)
	{
		CHECK_POINTER(HitResult.GetActor())
		ACHCharacterBase* Player = Cast<ACHCharacterBase>(HitResult.GetActor()); //UGameplayStatics::GetPlayerCharacter(this,0));
		
		if(!Player)
		{
			return;
		}
		Execute_ApplyDamageToActor(this, Player);
		
		CHECK_POINTER(WeaponComponent)
		CHECK_POINTER(WeaponComponent->GetWeaponData().HitVFX)

		UParticleSystem* VFX = WeaponComponent->GetWeaponData().HitVFX;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFX, HitResult.ImpactPoint, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
	}

	// One bullet was shot
	BulletsLeft--;
}
