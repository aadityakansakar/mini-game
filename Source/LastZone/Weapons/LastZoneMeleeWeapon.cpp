// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneMeleeWeapon.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "LastZone/LastZone.h"

ALastZoneMeleeWeapon::ALastZoneMeleeWeapon()
{
	AttackRange = 180.0f;
	AttackRadius = 40.0f;

	WeaponStats.WeaponName = FText::FromString(TEXT("Tactical Combat Machete"));
	WeaponStats.WeaponType = EWeaponType::Melee;
	WeaponStats.AmmoType = EAmmoType::None;
	WeaponStats.BaseDamage = 60.0f;
	WeaponStats.HeadshotMultiplier = 1.5f;
	WeaponStats.FireRateRPM = 80.0f;
	WeaponStats.MagazineCapacity = 1;
	WeaponStats.ReloadTimeSeconds = 0.1f;
}

void ALastZoneMeleeWeapon::ExecuteFireLogic(const FVector& MuzzleLocation, const FRotator& FireRotation)
{
	ServerPerformMeleeSweep(MuzzleLocation, FireRotation.Vector());
}

void ALastZoneMeleeWeapon::ServerPerformMeleeSweep_Implementation(const FVector& Origin, const FVector& Direction)
{
	UWorld* World = GetWorld();
	if (!World) return;

	FVector SweepEnd = Origin + (Direction * AttackRange);
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(AttackRadius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());

	FHitResult Hit;
	if (World->SweepSingleByChannel(Hit, Origin, SweepEnd, FQuat::Identity, COLLISION_WEAPON_TRACE, SphereShape, QueryParams))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			float FinalDamage = WeaponStats.BaseDamage;
			if (Hit.BoneName == FName(TEXT("head")))
			{
				FinalDamage *= WeaponStats.HeadshotMultiplier;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, FinalDamage, Hit.ImpactNormal, Hit, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}
}

bool ALastZoneMeleeWeapon::ServerPerformMeleeSweep_Validate(const FVector& Origin, const FVector& Direction)
{
	return true;
}

