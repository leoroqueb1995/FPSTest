// Fill out your copyright notice in the Description page of Project Settings.


#include "CHCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "FPSTest/Components/CHStatsComponent.h"
#include "FPSTest/Misc/CHUtils.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "FPSTest/FPSTest.h"
#include "FPSTest/Components/CHWeaponComponent.h"
#include "FPSTest/DataAssets/CHDACharacterConfig.h"
#include "FPSTest/Defines/CHGameplayTagDefines.h"
#include "FPSTest/Misc/CHFunctionUtils.h"
#include "FPSTest/PlayerController/CHPlayerController.h"
#include "FPSTest/RespawnPoints/CHRespawnPoint.h"
#include "FPSTest/Weapon/CHWeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

namespace CHCharacterBaseHelpers
{
	static const FName HandSocket = TEXT("hand_rSocket");
}

// Sets default values
ACHCharacterBase::ACHCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponActor = CreateDefaultSubobject<UChildActorComponent>(FName(TEXT("WeaponActor")));
	WeaponActor->SetupAttachment(RootComponent);

	StatsComponent = CreateDefaultSubobject<UCHStatsComponent>(FName(TEXT("StatsComponent")));

	CHECK_POINTER(CharacterConfig)
	GetCharacterMovement()->MaxWalkSpeed = CharacterConfig->JogSpeed;
}

void ACHCharacterBase::InitStats()
{
	OnCharacterDead.AddDynamic(this, &ThisClass::StartRespawnCooldown);
	
	CHECK_POINTER(GetStatsComponent())
	if (!GetStatsComponent()->OnStatDepleted.IsBound())
	{
		GetStatsComponent()->OnStatDepleted.AddDynamic(this, &ThisClass::OnStatDepleted);
	}
	
	CHECK_POINTER(CharacterConfig)
	CHECK_EMPTY_ARRAY(CharacterConfig->PlayerStats)
	for(const FStatData& Stat : CharacterConfig->PlayerStats)
	{
		GetStatsComponent()->AddStat(Stat);
	}
}

void ACHCharacterBase::InitWeaponActor()
{
	CHECK_POINTER(WeaponActor)
	WeaponActor->RegisterComponent();

	const FAttachmentTransformRules Rules =
		FAttachmentTransformRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepRelative,
		true);
	
	WeaponActor->AttachToComponent(GetMesh(), Rules, CHCharacterBaseHelpers::HandSocket);

	ACHWeaponBase* Weapon = GetPlayerWeapon();
	CHECK_POINTER_WITH_TEXT_VOID(Weapon, TEXT("CAUTION: This will cause that damage applied by this weapon {%s} won't be correctly associated with this character {%s}"), *WeaponActor->GetName(), *GetName())
	Weapon->SetWeaponOwner(this);

	CHECK_POINTER(CharacterConfig)
	AmmoInventory.Reset(); // Clear any previous value
	TMap<FGameplayTag, int32> AmmoData = CharacterConfig->MaxAmmoAmountPerGun;
	
	if(AmmoData.IsEmpty())
	{
		return;
	}

	for (TTuple<FGameplayTag, int32> Info : AmmoData)
	{
		if(!Info.Key.IsValid())
		{
			continue;
		}

		FPlayerAmmoData NewAmmoData = FPlayerAmmoData(Info.Key, Info.Value);
		
		if(Weapon->GetWeaponTag().MatchesTag(Info.Key))
		{
			NewAmmoData.CurrentAmmo = Weapon->GetRemainingBullets();
		}
		
		AmmoInventory.AddUnique(NewAmmoData);
	}
}

// Called when the game starts or when spawned
void ACHCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	InitStats();
	InitWeaponActor();
	SetPlayerName();
}

void ACHCharacterBase::StartRun()
{
	CHECK_POINTER(GetCharacterMovement())
	CHECK_POINTER(StatsComponent)
	CHECK_POINTER(CharacterConfig)
	
	if (!StatsComponent->HasStat(TAG_STAT_STAMINA))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: %s has no stamina stat."), *FString(__FUNCTION__), *GetName());
		return;
	}

	if (!StatsComponent->ConsumeStamina())
	{
		return;
	}
	
	GetCharacterMovement()->MaxWalkSpeed = CharacterConfig->SprintSpeed;
	bIsSprinting = true;

	// Force Camera to be on non-aiming distance
	if (GetIsAiming())
	{
		StopAim();
	}
}

void ACHCharacterBase::StopRun()
{
	CHECK_POINTER(GetCharacterMovement())
	CHECK_POINTER(CharacterConfig)
	GetCharacterMovement()->MaxWalkSpeed = CharacterConfig->JogSpeed;
	
	bIsSprinting = false;

	CHECK_POINTER(StatsComponent)
	StatsComponent->StopConsumingStamina();

	// Go back to aiming distance
	if (GetIsAiming())
	{
		StartAim();
	}
}

void ACHCharacterBase::StartAimAction()
{
	StartAim();
	bIsAiming = true;
}

void ACHCharacterBase::StartAim()
{
	CHECK_POINTER(GetCameraBoom())
	CHECK_POINTER(CharacterConfig)
	GetCameraBoom()->TargetArmLength = CharacterConfig->ZoomCameraDistance;
	//GetCameraBoom()->bUsePawnControlRotation = false;
}

void ACHCharacterBase::StopAim()
{
	CHECK_POINTER(GetCameraBoom())
	CHECK_POINTER(CharacterConfig)
	GetCameraBoom()->TargetArmLength = CharacterConfig->NormalCameraDistance;
	//GetCameraBoom()->bUsePawnControlRotation = true;
}

void ACHCharacterBase::StopAimAction()
{
	StopAim();
	bIsAiming = false;
}

void ACHCharacterBase::StartShoot()
{
	if(GetIsSprinting()) // Can't shoot while sprinting
	{
		return;
	}
	
	CHECK_POINTER(WeaponActor->GetChildActor())
	ACHWeaponBase* Weapon = GetPlayerWeapon();
	CHECK_POINTER(Weapon)

	Weapon->Shoot();
}

void ACHCharacterBase::StopShoot()
{
}

void ACHCharacterBase::ReloadGun()
{
	const ACHWeaponBase* Weapon = GetPlayerWeapon();
	
	CHECK_POINTER(Weapon)
	CHECK_POINTER(Weapon->GetWeaponComponent())
	CHECK_POINTER(GetWorld())

	if(GetCurrentAmmo(Weapon->GetWeaponTag()) == 0)
	{
		// Maybe some anim?
		return;
	}

	// Weapon is already reloaded
	if(Weapon->GetRemainingBullets() == Weapon->GetWeaponComponent()->GetMaxBulletsOnWeapon())
	{
		return;
	}
	
	bIsReloading = true;

	UAnimationAsset* ReloadAnim;
	if(GetIsAiming())
	{
		ReloadAnim = Weapon->GetWeaponComponent()->GetAimReloadPlayerAnimation();
	}
	else
	{
		ReloadAnim = Weapon->GetWeaponComponent()->GetHipReloadPlayerAnimation();
	} 

	CHECK_POINTER(ReloadAnim)
	
	OnCharacterReload.Broadcast(ReloadAnim);
	
	FTimerHandle ReloadTimerHandle;
	FTimerDelegate ReloadDelegate;
	ReloadDelegate.BindUObject(this, &ThisClass::ReloadGunDelegate);
	
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, ReloadDelegate, ReloadAnim->GetPlayLength()-0.1f, false);
}

void ACHCharacterBase::ReloadGunDelegate()
{
	bIsReloading = false;
	ACHWeaponBase* Weapon = GetPlayerWeapon();
	
	CHECK_POINTER(Weapon)
	const int32 CurrentAmmo = GetCurrentAmmo(Weapon->GetWeaponTag());
	
	if(CurrentAmmo == INDEX_NONE)
	{
		return;
	}
	
	Weapon->Reload(CurrentAmmo);
}

void ACHCharacterBase::AddAmmo(const FGameplayTag& AmmoType, int32 Amount)
{
	CHECK_VALID_TAG(AmmoType)
	CHECK_POINTER(CharacterConfig)
	
	for (FPlayerAmmoData& Data : AmmoInventory)
	{
		if(!Data.IsEqual(AmmoType))
		{
			continue;
		}
		
		Data.CurrentAmmo = FMath::Clamp(Data.CurrentAmmo + Amount, 0, Data.MaxAmmo);
		return;
	}
}

void ACHCharacterBase::RemoveAmmo(const FGameplayTag& AmmoType, int32 Amount)
{
	CHECK_VALID_TAG(AmmoType)
	CHECK_POINTER(CharacterConfig)
	
	for (FPlayerAmmoData& Data : AmmoInventory)
	{
		if(!Data.IsEqual(AmmoType))
		{
			continue;
		}
		
		Data.CurrentAmmo = FMath::Clamp(Data.CurrentAmmo - Amount, 0, Data.MaxAmmo);
		return;
	}
}

int32 ACHCharacterBase::GetCurrentAmmo(const FGameplayTag& AmmoType)
{
	for (FPlayerAmmoData& Data : AmmoInventory)
	{
		if(!Data.IsEqual(AmmoType))
		{
			continue;
		}
		
		return Data.CurrentAmmo;
	}

	return INDEX_NONE;
}

void ACHCharacterBase::FillAmmoInventory()
{
	CHECK_EMPTY_ARRAY(AmmoInventory)
	for (FPlayerAmmoData& Data : AmmoInventory)
	{
		Data.CurrentAmmo = Data.MaxAmmo;
	}

	CHECK_POINTER(GetPlayerWeapon())
	// Reload all weapons without consuming ammo
	GetPlayerWeapon()->Reload(100, false);
}

void ACHCharacterBase::OnStatDepleted(const FGameplayTag& StatDepleted)
{
	CHECK_VALID_TAG(StatDepleted)
	if (StatDepleted.MatchesTag(TAG_STAT_STAMINA))
	{
		StopRun();
	}

	if (StatDepleted.MatchesTag(TAG_STAT_HEALTH))
	{
		Dead();
	}
}

void ACHCharacterBase::Dead()
{
	CHECK_POINTER(GetMesh())
	SetIsDead(true);

	StopRun();
	StopAimAction();
	
	OnCharacterDead.Broadcast();
	
	const ACHPlayerController* PC = UCHFunctionUtils::GetCHPlayerController(this);
	CHECK_POINTER(PC)
	PC->ReplaceMappingContext(ECHGameState::DEAD, CharacterConfig);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	GetMesh()->SetAllBodiesSimulatePhysics(true);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	CHECK_POINTER(GetMovementComponent())
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	CHECK_POINTER(GetCapsuleComponent())
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACHCharacterBase::StartRespawnCooldown()
{
	CHECK_POINTER(GetWorld())
	CHECK_POINTER(CharacterConfig)
	
	FTimerHandle RespawnTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, [&]
	{
		SetCanRespawn(true);
	}, CharacterConfig->RespawnCooldownTime, false);
}

void ACHCharacterBase::Respawn()
{
	if(!PlayerCanRespawn())
	{
		return;
	}
	
	const ACHPlayerController* PC = UCHFunctionUtils::GetCHPlayerController(this);
	CHECK_POINTER(PC)
	PC->ReplaceMappingContext(ECHGameState::PLAY, CharacterConfig);
	
	CHECK_POINTER(GetMesh())
	GetMesh()->SetAllBodiesSimulatePhysics(false);

	CHECK_POINTER(GetCapsuleComponent())
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	GetMesh()->SetRelativeLocation(FVector(0,0,-90));
	GetMesh()->SetRelativeRotation(FRotator(0,-90,0));

	CHECK_POINTER(GetStatsComponent())
	GetStatsComponent()->ResetStats();

	const FTransform RandomSpawnPoint = GetRandomSpawnPoint();
	SetActorLocationAndRotation(RandomSpawnPoint.GetLocation(), RandomSpawnPoint.GetRotation());

	SetCanRespawn(false);
}

// Called every frame
void ACHCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACHCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Triggered, this, &ThisClass::StartRun);
		EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Completed, this, &ThisClass::StopRun);

		EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Started, this, &ThisClass::StartAimAction);
		EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ThisClass::StopAimAction);

		EnhancedInputComponent->BindAction(IA_Shoot, ETriggerEvent::Started, this, &ThisClass::StartShoot);
		EnhancedInputComponent->BindAction(IA_Shoot, ETriggerEvent::Completed, this, &ThisClass::StopShoot);

		EnhancedInputComponent->BindAction(IA_Respawn, ETriggerEvent::Triggered, this, &ThisClass::Respawn);

		EnhancedInputComponent->BindAction(IA_Reload, ETriggerEvent::Triggered, this, &ThisClass::ReloadGun);
	}
}

void ACHCharacterBase::SetPlayerName()
{
	CHECK_POINTER(CharacterConfig)
	
	PlayerName = CharacterConfig->GetRandomName();
}

FTransform ACHCharacterBase::GetRandomSpawnPoint() const
{
	CHECK_POINTER(GetWorld(), FTransform())
	
	TArray<AActor*> RespawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACHRespawnPoint::StaticClass(), RespawnPoints);
	CHECK_EMPTY_ARRAY(RespawnPoints, FTransform())
	
	const int32 RandomIndex = FMath::RandRange(0, RespawnPoints.Num()-1);
	const ACHRespawnPoint* RandomSpawn = Cast<ACHRespawnPoint>(RespawnPoints[RandomIndex]);
	CHECK_POINTER(RandomSpawn, FTransform())
	
	return RandomSpawn->GetSpawnReference();
}

void ACHCharacterBase::OnDamageTaken_Implementation(AActor* DamageInstigator, float DamageReceived)
{
	ICHDamageInterface::OnDamageTaken_Implementation(DamageInstigator, DamageReceived);

	// TODO: Hit reaction

	if(StatsComponent->GetCurrentValue(TAG_STAT_HEALTH) <= 0)
	{
		ACHCharacterBase* Causer = Cast<ACHCharacterBase>(DamageInstigator);
		CHECK_POINTER(Causer)
		
		Causer->AddSinglePoint();
	}
}

float ACHCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CHECK_POINTER_WITH_TEXT_RET(GetStatsComponent(), INDEX_NONE, TEXT(" %s will not receive any damage"), *GetName());

	if (!GetStatsComponent()->HasStat(TAG_STAT_HEALTH))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: %s does not have a health stat"), *FString(__FUNCTION__), *GetName())
		return INDEX_NONE;
	}

	const float HealthBeforeDamage = GetStatsComponent()->GetCurrentValue(TAG_STAT_HEALTH);

	if (HealthBeforeDamage == static_cast<float>(INDEX_NONE))
	{
		return INDEX_NONE;
	}

	GetStatsComponent()->SetCurrentValue(TAG_STAT_HEALTH, HealthBeforeDamage - DamageAmount);

	Execute_OnDamageTaken(this, DamageCauser, DamageAmount);

	// NOTE: No need to check death as it will be managed by OnStatDepleted delegate

	return DamageAmount;
}

void ACHCharacterBase::AddSinglePoint()
{
	Points++;
	Points = FMath::Clamp(Points, 0,5);

	if(Points == 5)
	{
		UE_LOG(LogTemp, Error, TEXT("%s WIN"), *GetName());
		// TODO: WIN
	}
}

ACHWeaponBase* ACHCharacterBase::GetPlayerWeapon() const
{
	return Cast<ACHWeaponBase>(WeaponActor->GetChildActor());
}
