// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LastZoneVehicleBase.h"
#include "LastZoneBikeVehicle.generated.h"

UCLASS()
class LASTZONE_API ALastZoneBikeVehicle : public ALastZoneVehicleBase
{
	GENERATED_BODY()

public:
	ALastZoneBikeVehicle();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Bike")
	float MaxLeanAngle;

	virtual void UpdateVehiclePhysics(float DeltaTime) override;
};

