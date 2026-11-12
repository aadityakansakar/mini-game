// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneBoatVehicle.h"
#include "Components/SkeletalMeshComponent.h"

ALastZoneBoatVehicle::ALastZoneBoatVehicle()
{
	MaxHealth = 1100.0f;
	CurrentHealth = MaxHealth;
	MaxSeats = 4;
	WaterLevelZ = 0.0f; // Sea level Z coordinate
	BuoyancyForce = 1500000.0f;
}

void ALastZoneBoatVehicle::UpdateVehiclePhysics(float DeltaTime)
{
	Super::UpdateVehiclePhysics(DeltaTime);

	if (VehicleMesh && VehicleMesh->IsSimulatingPhysics() && HasAuthority())
	{
		float VehicleZ = GetActorLocation().Z;
		float SubmergedDepth = WaterLevelZ - VehicleZ;

		if (SubmergedDepth > 0.0f)
		{
			// Water buoyancy pushes boat up to water level
			float BuoyantPush = FMath::Clamp(SubmergedDepth / 80.0f, 0.0f, 2.5f) * BuoyancyForce;
			VehicleMesh->AddForce(FVector(0.0f, 0.0f, BuoyantPush));

			// Water linear & angular drag
			FVector LinVel = VehicleMesh->GetPhysicsLinearVelocity();
			VehicleMesh->AddForce(-LinVel * 600.0f);

			FVector AngVel = VehicleMesh->GetPhysicsAngularVelocityInDegrees();
			VehicleMesh->AddTorqueInDegrees(-AngVel * 300.0f);
		}
	}
}

