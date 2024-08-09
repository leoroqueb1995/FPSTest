// Fill out your copyright notice in the Description page of Project Settings.


#include "CHCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "FPSTest/Components/CHStatsComponent.h"
#include "FPSTest/Misc/CHUtils.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "FPSTest/Defines/CHGameplayTagDefines.h"
#include "FPSTest/Weapon/CHWeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

namespace CHCharacterBaseHelpers
{
	static const FName HandSocket = TEXT("hand_rSocket");
	static constexpr float DefaultSpeed = 400.f;
	static constexpr float SprintSpeed = 1200.f;
}

// Sets default values
ACHCharacterBase::ACHCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponActor = CreateDefaultSubobject<UChildActorComponent>(FName(TEXT("WeaponActor")));
	WeaponActor->SetupAttachment(RootComponent);

	StatsComponent = CreateDefaultSubobject<UCHStatsComponent>(FName(TEXT("StatsComponent")));

	GetCharacterMovement()->MaxWalkSpeed = CHCharacterBaseHelpers::DefaultSpeed;
}

void ACHCharacterBase::InitStats()
{
	CHECK_POINTER(StatsComponent)
	if (!StatsComponent->OnStatDepleted.IsBound())
	{
		StatsComponent->OnStatDepleted.AddDynamic(this, &ThisClass::OnStatDepleted);
	}
	
	StatsComponent->AddStat(FStatData(TAG_STAT_HEALTH, 100.f));
	StatsComponent->AddStat(FStatData(TAG_STAT_STAMINA, 100.f));
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

	ACHWeaponBase* Weapon = Cast<ACHWeaponBase>(WeaponActor->GetChildActor());
	CHECK_POINTER_WITH_TEXT_VOID(Weapon, TEXT("CAUTION: This will cause that damage applied by this weapon {%s} won't be correctly associated with this character {%s}"), *WeaponActor->GetName(), *GetName())
	Weapon->SetWeaponOwner(this);
}

// Called when the game starts or when spawned
void ACHCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	InitStats();
	InitWeaponActor();
}

void ACHCharacterBase::StartRun()
{
	CHECK_POINTER(GetCharacterMovement())
	CHECK_POINTER(StatsComponent)

	if (!StatsComponent->HasStat(TAG_STAT_STAMINA))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: %s has no stamina stat."), *FString(__FUNCTION__), *GetName());
		return;
	}

	if (!StatsComponent->ConsumeStamina())
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = CHCharacterBaseHelpers::SprintSpeed;
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
	GetCharacterMovement()->MaxWalkSpeed = CHCharacterBaseHelpers::DefaultSpeed;
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
	GetCameraBoom()->TargetArmLength = 50;
	//GetCameraBoom()->bUsePawnControlRotation = false;
}

void ACHCharacterBase::StopAim()
{
	CHECK_POINTER(GetCameraBoom())
	GetCameraBoom()->TargetArmLength = 150;
	//GetCameraBoom()->bUsePawnControlRotation = true;
}

void ACHCharacterBase::StopAimAction()
{
	StopAim();
	bIsAiming = false;
}

void ACHCharacterBase::StartShoot()
{
	CHECK_POINTER(WeaponActor->GetChildActor())
	ACHWeaponBase* Weapon = Cast<ACHWeaponBase>(WeaponActor->GetChildActor());
	CHECK_POINTER(Weapon)

	Weapon->Shoot();
}

void ACHCharacterBase::StopShoot()
{
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
	}
}

void ACHCharacterBase::SetPlayerName()
{
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
