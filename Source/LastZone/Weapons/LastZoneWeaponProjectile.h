// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LastZoneWeaponBase.h"
#include "LastZoneWeaponProjectile.generated.h"

class ALastZoneProjectile;

UCLASS()
class LASTZONE_API ALastZoneWeaponProjectile : public ALastZoneWeaponBase
{
	GENERATED_BODY()

public:
	ALastZoneWeaponProjectile();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ALastZoneProjectile> ProjectileClass;

	virtual void ExecuteFireLogic(const FVector& MuzzleLocation, const FRotator& FireRotation) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnProjectile(const FVector& MuzzleLocation, const FRotator& FireRotation);
};

