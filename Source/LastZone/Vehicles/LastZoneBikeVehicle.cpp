// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneBikeVehicle.h"
#include "Components/SkeletalMeshComponent.h"

ALastZoneBikeVehicle::ALastZoneBikeVehicle()
{
	MaxHealth = 800.0f;
	CurrentHealth = MaxHealth;
	MaxSeats = 2; // Driver + passenger
	MaxLeanAngle = 35.0f;
}

void ALastZoneBikeVehicle::UpdateVehiclePhysics(float DeltaTime)
{
	Super::UpdateVehiclePhysics(DeltaTime);

	// Bike lean physics based on steering input and speed
	if (VehicleMesh && VehicleMesh->IsSimulatingPhysics() && HasAuthority())
	{
		float TargetRoll = SteeringInput * MaxLeanAngle * FMath::Clamp(GetForwardSpeedKmH() / 60.0f, 0.0f, 1.0f);
		FRotator CurrentRot = GetActorRotation();
		FRotator TargetRot = CurrentRot;
		TargetRot.Roll = TargetRoll;

		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 6.0f);
		SetActorRotation(NewRot);
	}
}

