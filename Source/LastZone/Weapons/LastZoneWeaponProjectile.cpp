// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneWeaponProjectile.h"
#include "LastZoneProjectile.h"
#include "Engine/World.h"

ALastZoneWeaponProjectile::ALastZoneWeaponProjectile()
{
	ProjectileClass = ALastZoneProjectile::StaticClass();

	WeaponStats.WeaponName = FText::FromString(TEXT("LZ-416 Assault Rifle"));
	WeaponStats.WeaponType = EWeaponType::AssaultRifle;
	WeaponStats.AmmoType = EAmmoType::Ammo_556mm;
	WeaponStats.BaseDamage = 41.0f;
	WeaponStats.HeadshotMultiplier = 2.3f;
	WeaponStats.FireRateRPM = 700.0f;
	WeaponStats.MagazineCapacity = 30;
	WeaponStats.ReloadTimeSeconds = 2.2f;
	WeaponStats.MuzzleVelocityMetersPerSec = 880.0f;
	WeaponStats.BulletGravityScale = 1.0f;
	WeaponStats.EffectiveRangeMeters = 600.0f;
	WeaponStats.RecoilPitchMin = 1.0f;
	WeaponStats.RecoilPitchMax = 1.6f;
	WeaponStats.RecoilYawMin = -0.4f;
	WeaponStats.RecoilYawMax = 0.6f;
	WeaponStats.BaseSpreadAngle = 0.6f;
	WeaponStats.AimDownSightsFOV = 60.0f;
}

void ALastZoneWeaponProjectile::ExecuteFireLogic(const FVector& MuzzleLocation, const FRotator& FireRotation)
{
	ServerSpawnProjectile(MuzzleLocation, FireRotation);
}

void ALastZoneWeaponProjectile::ServerSpawnProjectile_Implementation(const FVector& MuzzleLocation, const FRotator& FireRotation)
{
	if (!ProjectileClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (UWorld* World = GetWorld())
	{
		ALastZoneProjectile* Projectile = World->SpawnActor<ALastZoneProjectile>(ProjectileClass, MuzzleLocation, FireRotation, SpawnParams);
		if (Projectile)
		{
			Projectile->InitializeProjectile(WeaponStats.BaseDamage, WeaponStats.HeadshotMultiplier, WeaponStats.MuzzleVelocityMetersPerSec, WeaponStats.BulletGravityScale, nullptr);
		}
	}
}

bool ALastZoneWeaponProjectile::ServerSpawnProjectile_Validate(const FVector& MuzzleLocation, const FRotator& FireRotation)
{
	return true;
}

