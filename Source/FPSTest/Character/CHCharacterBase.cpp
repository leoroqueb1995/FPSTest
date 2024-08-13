// Fill out your copyright notice in the Description page of Project Settings.


#include "CHCharacterBase.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "FPSTest/Components/CHStatsComponent.h"
#include "FPSTest/Misc/CHUtils.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "FPSTest/CHGameMode.h"
#include "FPSTest/FPSTest.h"
#include "FPSTest/Camera/CHSprintCameraShake.h"
#include "FPSTest/Components/CHWeaponComponent.h"
#include "FPSTest/DataAssets/CHDACharacterConfig.h"
#include "FPSTest/Defines/CHGameplayTagDefines.h"
#include "FPSTest/Misc/CHFunctionUtils.h"
#include "FPSTest/PlayerController/CHPlayerController.h"
#include "FPSTest/RespawnPoints/CHRespawnPoint.h"
#include "FPSTest/UI/CHEndGameWidget.h"
#include "FPSTest/UI/CHHUDWidget.h"
#include "FPSTest/Weapon/CHWeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

namespace CHCharacterBaseHelpers
{
	static const FName HandSocket = TEXT("hand_rSocket");
	static const FName ReloadNotify = TEXT("Reload");
	static const FName EquipWeaponNotify = TEXT("ChangeWeapon");
	static const FName MontagesGroup = TEXT("BlendableGroup");
}

// Sets default values
ACHCharacterBase::ACHCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponActor = CreateDefaultSubobject<UChildActorComponent>(FName(TEXT("WeaponActor")));
	WeaponActor->SetupAttachment(RootComponent);

	StatsComponent = CreateDefaultSubobject<UCHStatsComponent>(FName(TEXT("StatsComponent")));

	ConfigureCamera();
	
	ACHGameMode* GameMode = UCHFunctionUtils::GetCHGameMode(this);
	CHECK_POINTER(GameMode)
	GameMode->OnGameStart.AddDynamic(this, &ThisClass::OnGameStart);
	GameMode->OnGameEnd.AddDynamic(this, &ThisClass::OnGameEnd);
	
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
	for (const FStatData& Stat : CharacterConfig->PlayerStats)
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

	CHECK_EMPTY_ARRAY(AmmoData)
	for (TTuple<FGameplayTag, int32> Info : AmmoData)
	{
		if (!Info.Key.IsValid())
		{
			continue;
		}

		FPlayerAmmoData NewAmmoData = FPlayerAmmoData(Info.Key, Info.Value);

		// Weapons begins with a complete magazine loaded
		NewAmmoData.WeaponRemainingAmmo = Weapon->GetWeaponComponent()->GetMaxAmmoOnWeapon(Info.Key);

		AmmoInventory.AddUnique(NewAmmoData);
	}
}

void ACHCharacterBase::ConfigureCamera()
{
	USpringArmComponent* CameraBoomRef = GetCameraBoom();
	CHECK_POINTER(CameraBoomRef);
	
	CameraBoomRef->TargetArmLength = 300.0f; 
	CameraBoomRef->bUsePawnControlRotation = true; 
	CameraBoomRef->SocketOffset = FVector(0.0f, 100.0f, 50.0f);
	CameraBoomRef->bDoCollisionTest = true; 
	CameraBoomRef->bEnableCameraLag = true; 
	CameraBoomRef->CameraLagSpeed = 10.0f;

	UCameraComponent* FollowCameraRef = GetFollowCamera();
	CHECK_POINTER(FollowCameraRef)
	
	FollowCameraRef->bUsePawnControlRotation = false; 
	FollowCameraRef->FieldOfView = 90.0f;
}

void ACHCharacterBase::CreateHUD()
{
	CHECK_POINTER(HUDClass)
	
	ACHPlayerController* PlayerController = UCHFunctionUtils::GetCHPlayerController(this);
	CHECK_POINTER(PlayerController)
	
	HUD = CreateWidget<UCHHUDWidget>(PlayerController, HUDClass);
	CHECK_POINTER(HUD)
	HUD->InitializeHUD(this);
	HUD->AddToViewport();
}

void ACHCharacterBase::OnMontageNotifyReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (NotifyName.IsEqual(CHCharacterBaseHelpers::ReloadNotify))
	{
		OnReloadNotify();
	}
	
	if (NotifyName.IsEqual(CHCharacterBaseHelpers::EquipWeaponNotify))
	{
		OnEquipWeaponNotify();
	}
}

// Called when the game starts or when spawned
void ACHCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	InitStats();
	InitWeaponActor();
	SetPlayerName();
	CreateHUD();
	
	CHECK_POINTER(GetMesh())
	CHECK_POINTER(GetMesh()->GetAnimInstance())
	GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &ThisClass::OnMontageNotifyReceived);
}

void ACHCharacterBase::StartRun()
{
	CHECK_POINTER(GetCharacterMovement())
	CHECK_POINTER(StatsComponent)
	CHECK_POINTER(CharacterConfig)
	CHECK_POINTER(GetHUD())
	GetHUD()->ShowStamina();

	CHECK_POINTER(GetMesh())
	CHECK_POINTER(GetMesh()->GetAnimInstance())
	
	GetMesh()->GetAnimInstance()->StopSlotAnimation(0.1f, CHCharacterBaseHelpers::MontagesGroup);
	
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

void ACHCharacterBase::ShakeCamera()
{
	CHECK_POINTER(SprintCameraShakeClass)
	CHECK_POINTER(GetCharacterMovement())
	ACHPlayerController* PC = UCHFunctionUtils::GetCHPlayerController(this);
	CHECK_POINTER(PC)
	
	if(!GetCharacterMovement()->Velocity.IsNearlyZero())
	{
		if(bCameraIsShaking)
		{
			return;
		}
		PC->ClientStartCameraShake(SprintCameraShakeClass);
		bCameraIsShaking = true;
	}
	else
	{
		if(!bCameraIsShaking)
		{
			return;
		}
		StopRun();
		PC->ClientStopCameraShake(SprintCameraShakeClass);
		bCameraIsShaking = false;
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
	
	CHECK_POINTER(SprintCameraShakeClass)
	ACHPlayerController* PC = UCHFunctionUtils::GetCHPlayerController(this);
	CHECK_POINTER(PC)
	PC->ClientStopCameraShake(SprintCameraShakeClass);
	bCameraIsShaking = false;
}

void ACHCharacterBase::StartAimAction()
{
	StartAim();
	bIsAiming = true;
}

void ACHCharacterBase::StartAim()
{
	CHECK_POINTER(GetFollowCamera())
	CHECK_POINTER(CharacterConfig)
	GetFollowCamera()->FieldOfView = CharacterConfig->ZoomCameraFOV;

}

void ACHCharacterBase::StopAim()
{
	CHECK_POINTER(GetFollowCamera())
	CHECK_POINTER(CharacterConfig)
	GetFollowCamera()->FieldOfView = CharacterConfig->NormalCameraFOV;
}

void ACHCharacterBase::StopAimAction()
{
	StopAim();
	bIsAiming = false;
}

void ACHCharacterBase::StartShoot()
{
	CHECK_POINTER(GetMesh())
	CHECK_POINTER(GetMesh()->GetAnimInstance())
	
	// Stop any reload/equip montage that could be playing
	GetMesh()->GetAnimInstance()->Montage_StopGroupByName(0.1f, CHCharacterBaseHelpers::MontagesGroup);

	if (GetIsSprinting()) // Can't shoot while sprinting
	{
		return;
	}

	if (!bCanShoot)
	{
		return;
	}
	
	ACHWeaponBase* Weapon = GetPlayerWeapon();
	CHECK_POINTER(Weapon)
	CHECK_POINTER(Weapon->GetWeaponComponent())

	if (Weapon->GetWeaponComponent()->GetWeaponData().FireMode == ECHFireMode::SINGLE_SHOT)
	{
		Weapon->Shoot();
		StopShoot();
		return;
	}

	CHECK_POINTER(GetWorld())
	float FireRateTimer = 1.f;
	switch (Weapon->GetWeaponComponent()->GetWeaponData().WeaponFireRate)
	{
	case ECHFireRate::FAST:
		FireRateTimer = FAST_FIRE_RATE;
		break;
	case ECHFireRate::MEDIUM:
		FireRateTimer = MEDIUM_FIRE_RATE;
		break;
	case ECHFireRate::SLOW:
		FireRateTimer = SLOW_FIRE_RATE
		break;
	}

	Weapon->Shoot();
	GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]
	{
		CHECK_POINTER(GetPlayerWeapon())

		if (!bCanShoot)
		{
			return;
		}

		if (GetPlayerWeapon()->GetRemainingBullets() == 0)
		{
			StopShoot();
		}
		GetPlayerWeapon()->Shoot();
	}, FireRateTimer, true);
}

void ACHCharacterBase::StopShoot()
{
	CHECK_POINTER(GetWorld())
	CHECK_POINTER(GetPlayerWeapon())
	CHECK_POINTER(GetPlayerWeapon()->GetWeaponComponent())

	bCanShoot = false;

	float FireRateTimer = 1.f;
	switch (GetPlayerWeapon()->GetWeaponComponent()->GetWeaponData().WeaponFireRate)
	{
	// Fast rate is so fast that setting a timer won't work.
	case ECHFireRate::FAST:
		bCanShoot = true;
		if (!GetWorld()->GetTimerManager().TimerExists(ShootTimerHandle))
		{
			return;
		}
		GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
		return;
	case ECHFireRate::MEDIUM:
		FireRateTimer = MEDIUM_FIRE_RATE;
		break;
	case ECHFireRate::SLOW:
		FireRateTimer = SLOW_FIRE_RATE;
		break;
	}

	// Forbid click & release for fast-shooting
	if (!GetWorld()->GetTimerManager().TimerExists(ShootCooldown))
	{
		GetWorld()->GetTimerManager().SetTimer(ShootCooldown, [this]
		{
			bCanShoot = true;
		}, FireRateTimer, false);
	}

	if (GetPlayerWeapon()->GetWeaponComponent()->GetWeaponData().FireMode == ECHFireMode::SINGLE_SHOT)
	{
		return;
	}

	// Clean shooting timer in case fire mode is automatic
	if (!GetWorld()->GetTimerManager().TimerExists(ShootTimerHandle))
	{
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
}

void ACHCharacterBase::ReloadGun()
{
	const ACHWeaponBase* Weapon = GetPlayerWeapon();

	CHECK_POINTER(Weapon)
	CHECK_POINTER(Weapon->GetWeaponComponent())

	CHECK_POINTER(GetMesh())

	if (GetCurrentAmmo(Weapon->GetWeaponTag()) == 0)
	{
		// Maybe some anim?
		return;
	}

	// Weapon is already reloaded
	if (Weapon->GetRemainingBullets() == Weapon->GetWeaponComponent()->GetMaxBulletsOnWeapon())
	{
		return;
	}

	bIsReloading = true;

	UAnimMontage* ReloadAnim = Weapon->GetWeaponComponent()->GetReloadAnimation(!GetIsAiming());
	CHECK_POINTER(ReloadAnim)

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	CHECK_POINTER(AnimInstance)
	
	AnimInstance->Montage_Play(ReloadAnim);
}

void ACHCharacterBase::OnReloadNotify()
{
	bIsReloading = false;
	ACHWeaponBase* Weapon = GetPlayerWeapon();

	CHECK_POINTER(Weapon)
	const int32 CurrentAmmo = GetCurrentAmmo(Weapon->GetWeaponTag());

	if (CurrentAmmo == INDEX_NONE)
	{
		return;
	}

	Weapon->Reload(CurrentAmmo);
}

void ACHCharacterBase::ChangeToWeaponOne()
{
	ChangeWeaponActorClass(ECHWeaponSelected::SMG);
}

void ACHCharacterBase::ChangeToWeaponTwo()
{
	ChangeWeaponActorClass(ECHWeaponSelected::PISTOL);
}

void ACHCharacterBase::ChangeToWeaponThree()
{
	ChangeWeaponActorClass(ECHWeaponSelected::SHOTGUN);
}

void ACHCharacterBase::OnEquipWeaponNotify()
{
	CHECK_POINTER(WeaponActor)
	WeaponActor->SetVisibility(true, true);
	
	CHECK_POINTER(GetPlayerWeapon())
	GetPlayerWeapon()->SetWeaponOwner(this);
	const int32 WeaponAmmo = GetWeaponCurrentAmmo(GetPlayerWeapon()->GetWeaponTag());
	if (WeaponAmmo == INDEX_NONE)
	{
		return;
	}

	GetPlayerWeapon()->SetWeaponBullets(WeaponAmmo);

	CHECK_POINTER(HUD)
	HUD->UpdateWeaponInfo();

	GetWorld()->GetTimerManager().ClearTimer(ShootCooldown);
}

void ACHCharacterBase::ChangeWeaponActorClass_Implementation(const ECHWeaponSelected NewWeaponClass)
{
	// Do nothing if you are trying to equip the weapon you already have equipped
	if(UCHFunctionUtils::GetGameplayTagAssociatedWithWeaponSelected(NewWeaponClass).MatchesTag(GetPlayerWeapon()->GetWeaponTag()))
	{
		return;
	}
	
	CHECK_POINTER(GetMesh())
	CHECK_POINTER(GetMesh()->GetAnimInstance())
	CHECK_POINTER(CharacterConfig)

	UAnimMontage* ChangeWeaponMontage = CharacterConfig->GetEquipMontage(NewWeaponClass);
	CHECK_POINTER(ChangeWeaponMontage)
	
	GetMesh()->GetAnimInstance()->Montage_Play(ChangeWeaponMontage);
}

void ACHCharacterBase::AddAmmo(const FGameplayTag& AmmoType, int32 Amount)
{
	CHECK_VALID_TAG(AmmoType)
	CHECK_POINTER(CharacterConfig)

	for (FPlayerAmmoData& Data : AmmoInventory)
	{
		if (!Data.IsEqual(AmmoType))
		{
			continue;
		}

		Data.PlayerCurrentAmmo = FMath::Clamp(Data.PlayerCurrentAmmo + Amount, 0, Data.MaxAmmo);
		return;
	}
}

void ACHCharacterBase::RemoveAmmo(const FGameplayTag& AmmoType, int32 Amount)
{
	CHECK_VALID_TAG(AmmoType)
	CHECK_POINTER(CharacterConfig)

	for (FPlayerAmmoData& Data : AmmoInventory)
	{
		if (!Data.IsEqual(AmmoType))
		{
			continue;
		}

		Data.PlayerCurrentAmmo = FMath::Clamp(Data.PlayerCurrentAmmo - Amount, 0, Data.MaxAmmo);
		return;
	}
}

int32 ACHCharacterBase::GetCurrentAmmo(const FGameplayTag& AmmoType)
{
	for (FPlayerAmmoData& Data : AmmoInventory)
	{
		if (!Data.IsEqual(AmmoType))
		{
			continue;
		}

		return Data.PlayerCurrentAmmo;
	}

	return INDEX_NONE;
}

int32 ACHCharacterBase::GetWeaponCurrentAmmo(const FGameplayTag& WeaponTag)
{
	for (FPlayerAmmoData& Data : AmmoInventory)
	{
		if (!Data.IsEqual(WeaponTag))
		{
			continue;
		}

		return Data.WeaponRemainingAmmo;
	}

	return INDEX_NONE;
}

void ACHCharacterBase::UpdateWeaponAmmo(const FGameplayTag& WeaponTag, int32 NewValue)
{
	for (FPlayerAmmoData& Data : AmmoInventory)
	{
		if (!Data.IsEqual(WeaponTag))
		{
			continue;
		}

		Data.WeaponRemainingAmmo = NewValue;
		break;
	}
}

void ACHCharacterBase::FillAmmoInventory()
{
	CHECK_EMPTY_ARRAY(AmmoInventory)
	for (FPlayerAmmoData& Data : AmmoInventory)
	{
		Data.PlayerCurrentAmmo = Data.MaxAmmo;
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

void ACHCharacterBase::PlayRandomHitMontage()
{
	CHECK_POINTER(CharacterConfig)
	CHECK_EMPTY_ARRAY(CharacterConfig->HitMontages)
	CHECK_POINTER(GetMesh())
	CHECK_POINTER(GetMesh()->GetAnimInstance())

	const int32 RandomIndex = FMath::RandRange(0, CharacterConfig->HitMontages.Num()-1);
	GetMesh()->GetAnimInstance()->Montage_Play(CharacterConfig->HitMontages[RandomIndex]);
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
	if (!PlayerCanRespawn())
	{
		return;
	}
	
	OnCharacterRespawn.Broadcast();
	
	const ACHPlayerController* PC = UCHFunctionUtils::GetCHPlayerController(this);
	CHECK_POINTER(PC)
	PC->ReplaceMappingContext(ECHGameState::PLAY, CharacterConfig);

	CHECK_POINTER(GetMesh())
	GetMesh()->SetAllBodiesSimulatePhysics(false);

	CHECK_POINTER(GetCapsuleComponent())
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

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
		EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Triggered, this, &ThisClass::ShakeCamera);
		EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Completed, this, &ThisClass::StopRun);

		EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Started, this, &ThisClass::StartAimAction);
		EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ThisClass::StopAimAction);

		EnhancedInputComponent->BindAction(IA_Shoot, ETriggerEvent::Started, this, &ThisClass::StartShoot);
		EnhancedInputComponent->BindAction(IA_Shoot, ETriggerEvent::Completed, this, &ThisClass::StopShoot);

		EnhancedInputComponent->BindAction(IA_Respawn, ETriggerEvent::Triggered, this, &ThisClass::Respawn);

		EnhancedInputComponent->BindAction(IA_Reload, ETriggerEvent::Triggered, this, &ThisClass::ReloadGun);

		EnhancedInputComponent->BindAction(IA_Weapon1, ETriggerEvent::Triggered, this, &ThisClass::ChangeToWeaponOne);

		EnhancedInputComponent->BindAction(IA_Weapon2, ETriggerEvent::Triggered, this, &ThisClass::ChangeToWeaponTwo);

		EnhancedInputComponent->BindAction(IA_Weapon3, ETriggerEvent::Triggered, this, &ThisClass::ChangeToWeaponThree);

		EnhancedInputComponent->BindAction(IA_TableScore, ETriggerEvent::Started, this, &ThisClass::ShowTableScore);
		EnhancedInputComponent->BindAction(IA_TableScore, ETriggerEvent::Completed, this, &ThisClass::HideTableScore);
	}
}

void ACHCharacterBase::SetPlayerName()
{
	CHECK_POINTER(CharacterConfig)

	PlayerName = CharacterConfig->GetRandomName();
}

void ACHCharacterBase::OnGameStart()
{
	const ACHPlayerController* PlayerController = UCHFunctionUtils::GetCHPlayerController(this);
	CHECK_POINTER(PlayerController)
	
	// Avoid any type of input
	PlayerController->ClearMappings();
	
	CHECK_POINTER(HUD)
	HUD->OnGameStart();
}

void ACHCharacterBase::OnGameEnd(ACHCharacterBase* Winner)
{
	ACHPlayerController* PlayerController = UCHFunctionUtils::GetCHPlayerController(this);
	
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false); 
	
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;

	PlayerController->ReplaceMappingContext(ECHGameState::MENU, CharacterConfig);

	CHECK_POINTER(GetHUD())
	GetHUD()->OnGameEnd(Winner->PlayerName);
}

void ACHCharacterBase::ShowTableScore()
{
	CHECK_POINTER(GetHUD())
	GetHUD()->ShowTableScore(true);
}

void ACHCharacterBase::HideTableScore()
{
	CHECK_POINTER(GetHUD())
	GetHUD()->ShowTableScore(false);
}

FTransform ACHCharacterBase::GetRandomSpawnPoint() const
{
	CHECK_POINTER(GetWorld(), FTransform())

	TArray<AActor*> RespawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACHRespawnPoint::StaticClass(), RespawnPoints);
	CHECK_EMPTY_ARRAY(RespawnPoints, FTransform())

	const int32 RandomIndex = FMath::RandRange(0, RespawnPoints.Num() - 1);
	const ACHRespawnPoint* RandomSpawn = Cast<ACHRespawnPoint>(RespawnPoints[RandomIndex]);
	CHECK_POINTER(RandomSpawn, FTransform())

	return RandomSpawn->GetSpawnReference();
}

void ACHCharacterBase::OnDamageTaken_Implementation(AActor* DamageInstigator, float DamageReceived)
{
	ICHDamageInterface::OnDamageTaken_Implementation(DamageInstigator, DamageReceived);
	
	if (StatsComponent->GetCurrentValue(TAG_STAT_HEALTH) <= 0)
	{
		ACHCharacterBase* Causer = Cast<ACHCharacterBase>(DamageInstigator);
		CHECK_POINTER(Causer)
		Causer->AddSinglePoint();
		
		CHECK_POINTER(Causer->GetHUD())
		Causer->GetHUD()->SpawnHitMarker(true);
		return;
	}
	
	PlayRandomHitMontage();
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
	Points = FMath::Clamp(Points, 0, 5);

	if (Points == 5)
	{
		const ACHGameMode* GM = UCHFunctionUtils::GetCHGameMode(this);
		CHECK_POINTER(GM)

		GM->OnGameEnd.Broadcast(this);
	}
}

ACHWeaponBase* ACHCharacterBase::GetPlayerWeapon() const
{
	return Cast<ACHWeaponBase>(WeaponActor->GetChildActor());
}
