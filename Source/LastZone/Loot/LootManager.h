// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LastZone/LastZoneTypes.h"
#include "LootManager.generated.h"

class AGroundItemPickup;
class ASupplyDropCrate;

UCLASS()
class LASTZONE_API ALootManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ALootManager();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Loot|Config")
	TArray<FLastZoneItem> AvailableLootTable;

	UPROPERTY(EditDefaultsOnly, Category = "Loot|Config")
	TSubclassOf<AGroundItemPickup> PickupClass;

	UPROPERTY(EditDefaultsOnly, Category = "Loot|Config")
	TSubclassOf<ASupplyDropCrate> SupplyDropClass;

	UPROPERTY(EditDefaultsOnly, Category = "Loot|Config")
	float LootSpawnDensity;

	void PopulateDefaultLootTable();

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Loot")
	void SpawnLootAtLocation(const FVector& Location, int32 ItemCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Loot")
	void SpawnAirDrop(const FVector& DropCenter, float DropRadius = 500.0f);

	FLastZoneItem GetRandomLootItem();
};

