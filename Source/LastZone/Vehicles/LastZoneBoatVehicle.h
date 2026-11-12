// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LastZoneVehicleBase.h"
#include "LastZoneBoatVehicle.generated.h"

UCLASS()
class LASTZONE_API ALastZoneBoatVehicle : public ALastZoneVehicleBase
{
	GENERATED_BODY()

public:
	ALastZoneBoatVehicle();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Boat")
	float WaterLevelZ;

	UPROPERTY(EditDefaultsOnly, Category = "Boat")
	float BuoyancyForce;

	virtual void UpdateVehiclePhysics(float DeltaTime) override;
};

