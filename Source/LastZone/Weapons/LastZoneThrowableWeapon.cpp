// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneThrowableWeapon.h"
#include "LastZoneGrenadeProjectile.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

ALastZoneThrowableWeapon::ALastZoneThrowableWeapon()
{
	GrenadeClass = ALastZoneGrenadeProjectile::StaticClass();
	ThrowStrength = 2200.0f;

	WeaponStats.WeaponName = FText::FromString(TEXT("M67 Frag Grenade"));
	WeaponStats.WeaponType = EWeaponType::Throwable;
	WeaponStats.AmmoType = EAmmoType::None;
	WeaponStats.MagazineCapacity = 1;
	WeaponStats.ReloadTimeSeconds = 1.0f;
	WeaponStats.FireRateRPM = 30.0f;
}

void ALastZoneThrowableWeapon::ExecuteFireLogic(const FVector& MuzzleLocation, const FRotator& FireRotation)
{
	ServerThrowGrenade(MuzzleLocation, FireRotation.Vector());
}

void ALastZoneThrowableWeapon::ServerThrowGrenade_Implementation(const FVector& ThrowOrigin, const FVector& ThrowDirection)
{
	if (!GrenadeClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (UWorld* World = GetWorld())
	{
		ALastZoneGrenadeProjectile* Grenade = World->SpawnActor<ALastZoneGrenadeProjectile>(GrenadeClass, ThrowOrigin, ThrowDirection.Rotation(), SpawnParams);
		if (Grenade)
		{
			FVector Impulse = (ThrowDirection + FVector(0.0f, 0.0f, 0.2f)).GetSafeNormal() * ThrowStrength;
			Grenade->Throw(Impulse);
		}
	}
}

bool ALastZoneThrowableWeapon::ServerThrowGrenade_Validate(const FVector& ThrowOrigin, const FVector& ThrowDirection)
{
	return true;
}

