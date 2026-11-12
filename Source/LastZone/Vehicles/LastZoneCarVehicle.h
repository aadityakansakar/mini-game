// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LastZoneVehicleBase.h"
#include "LastZoneCarVehicle.generated.h"

UCLASS()
class LASTZONE_API ALastZoneCarVehicle : public ALastZoneVehicleBase
{
	GENERATED_BODY()

public:
	ALastZoneCarVehicle();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	float SuspensionStiffness;

	UPROPERTY(EditDefaultsOnly, Category = "Car")
	float MaxSteeringAngle;

	virtual void UpdateVehiclePhysics(float DeltaTime) override;
};

