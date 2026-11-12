// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneCarVehicle.h"
#include "Components/SkeletalMeshComponent.h"

ALastZoneCarVehicle::ALastZoneCarVehicle()
{
	MaxHealth = 1500.0f;
	CurrentHealth = MaxHealth;
	MaxSeats = 4;
	SuspensionStiffness = 6500.0f;
	MaxSteeringAngle = 35.0f;
}

void ALastZoneCarVehicle::UpdateVehiclePhysics(float DeltaTime)
{
	Super::UpdateVehiclePhysics(DeltaTime);

	// Raycast 4-wheel independent suspension simulation for off-road terrain
	if (VehicleMesh && VehicleMesh->IsSimulatingPhysics() && HasAuthority())
	{
		static const FVector WheelOffsets[4] = {
			FVector(140.0f, 85.0f, -20.0f),  // Front Right
			FVector(140.0f, -85.0f, -20.0f), // Front Left
			FVector(-140.0f, 85.0f, -20.0f), // Rear Right
			FVector(-140.0f, -85.0f, -20.0f) // Rear Left
		};

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		const float RestLength = 70.0f;
		const float SpringK = SuspensionStiffness;
		const float DamperC = 1200.0f;

		for (int32 i = 0; i < 4; ++i)
		{
			FVector WorldWheelPos = GetActorTransform().TransformPosition(WheelOffsets[i]);
			FVector DownDir = -GetActorUpVector();
			FHitResult Hit;

			if (GetWorld()->LineTraceSingleByChannel(Hit, WorldWheelPos, WorldWheelPos + (DownDir * RestLength), ECC_WorldStatic, Params))
			{
				float Compression = RestLength - Hit.Distance;
				float SpringForce = Compression * SpringK;

				// Relative upward velocity at wheel
				FVector PointVelocity = VehicleMesh->GetPhysicsLinearVelocityAtPoint(WorldWheelPos);
				float DampingForce = -FVector::DotProduct(PointVelocity, DownDir) * DamperC;

				FVector TotalUpForce = -DownDir * FMath::Max(0.0f, SpringForce + DampingForce);
				VehicleMesh->AddForceAtLocation(TotalUpForce, WorldWheelPos);
			}
		}
	}
}

