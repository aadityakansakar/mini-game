// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LastZoneWeaponBase.h"
#include "LastZoneMeleeWeapon.generated.h"

UCLASS()
class LASTZONE_API ALastZoneMeleeWeapon : public ALastZoneWeaponBase
{
	GENERATED_BODY()

public:
	ALastZoneMeleeWeapon();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float AttackRange;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float AttackRadius;

	virtual void ExecuteFireLogic(const FVector& MuzzleLocation, const FRotator& FireRotation) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPerformMeleeSweep(const FVector& Origin, const FVector& Direction);
};

