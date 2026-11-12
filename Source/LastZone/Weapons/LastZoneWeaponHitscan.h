// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LastZoneWeaponBase.h"
#include "LastZoneWeaponHitscan.generated.h"

UCLASS()
class LASTZONE_API ALastZoneWeaponHitscan : public ALastZoneWeaponBase
{
	GENERATED_BODY()

public:
	ALastZoneWeaponHitscan();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Hitscan")
	int32 PelletsPerShot;

	virtual void ExecuteFireLogic(const FVector& MuzzleLocation, const FRotator& FireRotation) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerProcessHitscan(const FVector& MuzzleLocation, const FRotator& FireRotation);
};

