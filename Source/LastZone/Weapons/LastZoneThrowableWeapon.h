// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LastZoneWeaponBase.h"
#include "LastZoneThrowableWeapon.generated.h"

class ALastZoneGrenadeProjectile;

UCLASS()
class LASTZONE_API ALastZoneThrowableWeapon : public ALastZoneWeaponBase
{
	GENERATED_BODY()

public:
	ALastZoneThrowableWeapon();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Throwable")
	TSubclassOf<ALastZoneGrenadeProjectile> GrenadeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Throwable")
	float ThrowStrength;

	virtual void ExecuteFireLogic(const FVector& MuzzleLocation, const FRotator& FireRotation) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerThrowGrenade(const FVector& ThrowOrigin, const FVector& ThrowDirection);
};

