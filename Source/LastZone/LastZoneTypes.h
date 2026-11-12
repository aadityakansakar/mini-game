// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LastZoneTypes.generated.h"

// Match state for Battle Royale flow
UENUM(BlueprintType)
enum class ELastZoneMatchState : uint8
{
	WaitingForPlayers UMETA(DisplayName = "Waiting For Players"),
	WarmupCountdown   UMETA(DisplayName = "Warmup Countdown"),
	PlaneDrop         UMETA(DisplayName = "Plane Drop"),
	ActiveBattle      UMETA(DisplayName = "Active Battle"),
	ZoneClosing       UMETA(DisplayName = "Zone Closing"),
	MatchEnded        UMETA(DisplayName = "Match Ended")
};

// Player movement & posture stances
UENUM(BlueprintType)
enum class EPlayerStance : uint8
{
	Standing   UMETA(DisplayName = "Standing"),
	Crouching  UMETA(DisplayName = "Crouching"),
	Prone      UMETA(DisplayName = "Prone")
};

UENUM(BlueprintType)
enum class EPlayerMovementState : uint8
{
	Idle       UMETA(DisplayName = "Idle"),
	Walking    UMETA(DisplayName = "Walking"),
	Sprinting  UMETA(DisplayName = "Sprinting"),
	Jumping    UMETA(DisplayName = "Jumping"),
	Falling    UMETA(DisplayName = "Falling"),
	Vaulting   UMETA(DisplayName = "Vaulting"),
	Swimming   UMETA(DisplayName = "Swimming"),
	Driving    UMETA(DisplayName = "Driving"),
	Ragdoll    UMETA(DisplayName = "Ragdoll")
};

// Weapon types & classification
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	AssaultRifle      UMETA(DisplayName = "Assault Rifle"),
	SniperRifle       UMETA(DisplayName = "Sniper Rifle"),
	SubmachineGun     UMETA(DisplayName = "Submachine Gun"),
	Shotgun           UMETA(DisplayName = "Shotgun"),
	Pistol            UMETA(DisplayName = "Pistol"),
	Melee             UMETA(DisplayName = "Melee"),
	Throwable         UMETA(DisplayName = "Throwable")
};

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	Single    UMETA(DisplayName = "Single"),
	Burst     UMETA(DisplayName = "Burst"),
	FullAuto  UMETA(DisplayName = "Full Auto")
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	None          UMETA(DisplayName = "None"),
	Ammo_556mm    UMETA(DisplayName = "5.56mm"),
	Ammo_762mm    UMETA(DisplayName = "7.62mm"),
	Ammo_9mm      UMETA(DisplayName = "9mm"),
	Ammo_12Gauge  UMETA(DisplayName = "12 Gauge"),
	Ammo_300Mag   UMETA(DisplayName = ".300 Magnum")
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common      UMETA(DisplayName = "Common"),
	Uncommon    UMETA(DisplayName = "Uncommon"),
	Rare        UMETA(DisplayName = "Rare"),
	Epic        UMETA(DisplayName = "Epic"),
	Legendary   UMETA(DisplayName = "Legendary")
};

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	Weapon      UMETA(DisplayName = "Weapon"),
	Ammo        UMETA(DisplayName = "Ammunition"),
	Armor       UMETA(DisplayName = "Armor Vest"),
	Helmet      UMETA(DisplayName = "Helmet"),
	Backpack    UMETA(DisplayName = "Backpack"),
	Medical     UMETA(DisplayName = "Medical Kit"),
	Boost       UMETA(DisplayName = "Energy Drink / Painkiller"),
	Attachment  UMETA(DisplayName = "Attachment")
};

UENUM(BlueprintType)
enum class EAttachmentSlot : uint8
{
	Muzzle    UMETA(DisplayName = "Muzzle"),
	Sight     UMETA(DisplayName = "Sight / Optic"),
	Magazine  UMETA(DisplayName = "Magazine"),
	Grip      UMETA(DisplayName = "Grip / Foregrip"),
	Stock     UMETA(DisplayName = "Stock")
};

// Core Weapon Parameters
USTRUCT(BlueprintType)
struct FWeaponStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FText WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	EWeaponType WeaponType = EWeaponType::AssaultRifle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	EAmmoType AmmoType = EAmmoType::Ammo_556mm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float BaseDamage = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float HeadshotMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float LimbMultiplier = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handling")
	float FireRateRPM = 600.0f; // Rounds per minute

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handling")
	int32 MagazineCapacity = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handling")
	float ReloadTimeSeconds = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ballistics")
	float MuzzleVelocityMetersPerSec = 880.0f; // Real-world ballistics m/s

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ballistics")
	float BulletGravityScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ballistics")
	float EffectiveRangeMeters = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float RecoilPitchMin = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float RecoilPitchMax = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float RecoilYawMin = -0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float RecoilYawMax = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float RecoilRecoverySpeed = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accuracy")
	float BaseSpreadAngle = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accuracy")
	float MovementSpreadMultiplier = 2.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optics")
	float AimDownSightsFOV = 65.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optics")
	float ADSSpeed = 0.22f; // Seconds to full ADS
};

// Safe Zone shrinking circle parameters
USTRUCT(BlueprintType)
struct FSafeZonePhase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	int32 PhaseNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	float WaitDurationSeconds = 120.0f; // Time before shrinking begins

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	float ShrinkDurationSeconds = 90.0f; // Shrink transit duration

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	float TargetRadiusMeters = 1500.0f; // Target radius in meters

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	float DamagePerSecond = 1.5f; // Health damage per second outside
};

// Item Definition for Inventory & Loot
USTRUCT(BlueprintType)
struct FLastZoneItem : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemCategory Category = EItemCategory::Medical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemRarity Rarity = EItemRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxStack = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Weight = 1.0f; // Inventory capacity cost

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ValueModifier = 0.0f; // E.g., heal amount, armor rating, damage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<class AActor> WorldMeshClass;
};

// Kill Feed Entry
USTRUCT(BlueprintType)
struct FKillFeedEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "KillFeed")
	FString KillerName;

	UPROPERTY(BlueprintReadOnly, Category = "KillFeed")
	FString VictimName;

	UPROPERTY(BlueprintReadOnly, Category = "KillFeed")
	FString WeaponName;

	UPROPERTY(BlueprintReadOnly, Category = "KillFeed")
	bool bIsHeadshot = false;

	UPROPERTY(BlueprintReadOnly, Category = "KillFeed")
	bool bEliminatedByZone = false;

	UPROPERTY(BlueprintReadOnly, Category = "KillFeed")
	float DistanceMeters = 0.0f;
};

// Match Results Summary
USTRUCT(BlueprintType)
struct FPlayerMatchResults
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Results")
	int32 Placement = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Results")
	int32 TotalKills = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Results")
	float DamageDealt = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Results")
	float SurvivalTimeSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Results")
	int32 Headshots = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Results")
	bool bIsWinner = false;
};

