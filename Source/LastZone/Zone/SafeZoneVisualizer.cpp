// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "SafeZoneVisualizer.h"
#include "Components/StaticMeshComponent.h"

ASafeZoneVisualizer::ASafeZoneVisualizer()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh"));
	CylinderMesh->SetCollisionProfileName(TEXT("NoCollision"));
	CylinderMesh->SetCastShadow(false);
	RootComponent = CylinderMesh;
}

void ASafeZoneVisualizer::UpdateVisuals(const FVector2D& CenterMeters, float RadiusMeters)
{
	// Convert meters to Unreal units (cm)
	FVector WorldPos(CenterMeters.X * 100.0f, CenterMeters.Y * 100.0f, 0.0f);
	SetActorLocation(WorldPos);

	// Standard cylinder mesh in UE is diameter 100 (radius 50), height 100
	// Radius scale: (RadiusMeters * 100) / 50 = RadiusMeters * 2
	float RadiusScale = (RadiusMeters * 100.0f) / 50.0f;
	float HeightScale = 200.0f; // Tall enough to cover island elevation

	SetActorScale3D(FVector(RadiusScale, RadiusScale, HeightScale));
}

