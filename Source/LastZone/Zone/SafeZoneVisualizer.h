// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SafeZoneVisualizer.generated.h"

class UStaticMeshComponent;

UCLASS()
class LASTZONE_API ASafeZoneVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	ASafeZoneVisualizer();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CylinderMesh;

public:
	UFUNCTION(BlueprintCallable, Category = "Zone")
	void UpdateVisuals(const FVector2D& CenterMeters, float RadiusMeters);
};

