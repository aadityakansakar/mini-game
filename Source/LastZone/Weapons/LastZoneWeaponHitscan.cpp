// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneWeaponHitscan.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "LastZone/LastZone.h"

ALastZoneWeaponHitscan::ALastZoneWeaponHitscan()
{
	PelletsPerShot = 9;

	WeaponStats.WeaponName = FText::FromString(TEXT("LZ-12 Pump Action Shotgun"));
	WeaponStats.WeaponType = EWeaponType::Shotgun;
	WeaponStats.AmmoType = EAmmoType::Ammo_12Gauge;
	WeaponStats.BaseDamage = 22.0f; // 22 * 9 pellets = ~198 max point blank damage
	WeaponStats.HeadshotMultiplier = 1.5f;
	WeaponStats.FireRateRPM = 75.0f;
	WeaponStats.MagazineCapacity = 5;
	WeaponStats.ReloadTimeSeconds = 3.5f;
	WeaponStats.EffectiveRangeMeters = 30.0f;
	WeaponStats.RecoilPitchMin = 3.5f;
	WeaponStats.RecoilPitchMax = 5.0f;
	WeaponStats.RecoilYawMin = -1.0f;
	WeaponStats.RecoilYawMax = 1.0f;
	WeaponStats.BaseSpreadAngle = 3.5f;
}

void ALastZoneWeaponHitscan::ExecuteFireLogic(const FVector& MuzzleLocation, const FRotator& FireRotation)
{
	ServerProcessHitscan(MuzzleLocation, FireRotation);
}

void ALastZoneWeaponHitscan::ServerProcessHitscan_Implementation(const FVector& MuzzleLocation, const FRotator& FireRotation)
{
	UWorld* World = GetWorld();
	if (!World) return;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	float MaxDistance = WeaponStats.EffectiveRangeMeters * 100.0f;

	for (int32 i = 0; i < PelletsPerShot; ++i)
	{
		FVector ShotDir = FMath::VRandCone(FireRotation.Vector(), FMath::DegreesToRadians(WeaponStats.BaseSpreadAngle));
		FVector TraceEnd = MuzzleLocation + (ShotDir * MaxDistance);

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, MuzzleLocation, TraceEnd, COLLISION_WEAPON_TRACE, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			{
				float PelletDamage = WeaponStats.BaseDamage;
				if (Hit.BoneName == FName(TEXT("head")))
				{
					PelletDamage *= WeaponStats.HeadshotMultiplier;
				}

				UGameplayStatics::ApplyPointDamage(HitActor, PelletDamage, Hit.ImpactNormal, Hit, GetInstigatorController(), this, UDamageType::StaticClass());
			}
		}
	}
}

bool ALastZoneWeaponHitscan::ServerProcessHitscan_Validate(const FVector& MuzzleLocation, const FRotator& FireRotation)
{
	return true;
}

