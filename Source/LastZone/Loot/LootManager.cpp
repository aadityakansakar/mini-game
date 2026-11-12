// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LootManager.h"
#include "GroundItemPickup.h"
#include "SupplyDropCrate.h"
#include "Engine/World.h"

ALootManager::ALootManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	PickupClass = AGroundItemPickup::StaticClass();
	SupplyDropClass = ASupplyDropCrate::StaticClass();
	LootSpawnDensity = 1.0f;
}

void ALootManager::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		PopulateDefaultLootTable();
	}
}

void ALootManager::PopulateDefaultLootTable()
{
	// Weapons
	FLastZoneItem AR;
	AR.ItemID = FName(TEXT("Weapon_LZ416"));
	AR.DisplayName = FText::FromString(TEXT("LZ-416 Assault Rifle"));
	AR.Category = EItemCategory::Weapon;
	AR.Rarity = EItemRarity::Rare;
	AR.Quantity = 1;
	AR.Weight = 25.0f;
	AvailableLootTable.Add(AR);

	FLastZoneItem Shotgun;
	Shotgun.ItemID = FName(TEXT("Weapon_LZ12"));
	Shotgun.DisplayName = FText::FromString(TEXT("LZ-12 Shotgun"));
	Shotgun.Category = EItemCategory::Weapon;
	Shotgun.Rarity = EItemRarity::Uncommon;
	Shotgun.Quantity = 1;
	Shotgun.Weight = 20.0f;
	AvailableLootTable.Add(Shotgun);

	// Ammo
	FLastZoneItem Ammo556;
	Ammo556.ItemID = FName(TEXT("Ammo_556"));
	Ammo556.DisplayName = FText::FromString(TEXT("5.56mm Ammo Box"));
	Ammo556.Category = EItemCategory::Ammo;
	Ammo556.Rarity = EItemRarity::Common;
	Ammo556.Quantity = 30;
	Ammo556.MaxStack = 180;
	Ammo556.Weight = 0.15f;
	AvailableLootTable.Add(Ammo556);

	FLastZoneItem Ammo12G;
	Ammo12G.ItemID = FName(TEXT("Ammo_12Gauge"));
	Ammo12G.DisplayName = FText::FromString(TEXT("12 Gauge Shells"));
	Ammo12G.Category = EItemCategory::Ammo;
	Ammo12G.Rarity = EItemRarity::Common;
	Ammo12G.Quantity = 10;
	Ammo12G.MaxStack = 50;
	Ammo12G.Weight = 0.25f;
	AvailableLootTable.Add(Ammo12G);

	// Medical
	FLastZoneItem FirstAid;
	FirstAid.ItemID = FName(TEXT("FirstAidKit"));
	FirstAid.DisplayName = FText::FromString(TEXT("First Aid Kit"));
	FirstAid.Category = EItemCategory::Medical;
	FirstAid.Rarity = EItemRarity::Uncommon;
	FirstAid.Quantity = 1;
	FirstAid.Weight = 10.0f;
	AvailableLootTable.Add(FirstAid);

	FLastZoneItem EnergyDrink;
	EnergyDrink.ItemID = FName(TEXT("EnergyDrink"));
	EnergyDrink.DisplayName = FText::FromString(TEXT("Energy Drink"));
	EnergyDrink.Category = EItemCategory::Boost;
	EnergyDrink.Rarity = EItemRarity::Common;
	EnergyDrink.Quantity = 1;
	EnergyDrink.Weight = 4.0f;
	AvailableLootTable.Add(EnergyDrink);

	// Armor
	FLastZoneItem VestL2;
	VestL2.ItemID = FName(TEXT("Vest_Level2"));
	VestL2.DisplayName = FText::FromString(TEXT("Police Tactical Vest (Lv 2)"));
	VestL2.Category = EItemCategory::Armor;
	VestL2.Rarity = EItemRarity::Rare;
	VestL2.Quantity = 1;
	VestL2.ValueModifier = 2.0f; // Level 2
	VestL2.Weight = 15.0f;
	AvailableLootTable.Add(VestL2);
}

FLastZoneItem ALootManager::GetRandomLootItem()
{
	if (AvailableLootTable.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, AvailableLootTable.Num() - 1);
		return AvailableLootTable[RandomIndex];
	}
	FLastZoneItem DefaultItem;
	DefaultItem.ItemID = FName(TEXT("Ammo_556"));
	DefaultItem.Quantity = 30;
	return DefaultItem;
}

void ALootManager::SpawnLootAtLocation(const FVector& Location, int32 ItemCount)
{
	if (!PickupClass || !HasAuthority()) return;

	for (int32 i = 0; i < ItemCount; ++i)
	{
		FVector SpawnLoc = Location + FVector(FMath::FRandRange(-50.0f, 50.0f), FMath::FRandRange(-50.0f, 50.0f), 10.0f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AGroundItemPickup* Pickup = GetWorld()->SpawnActor<AGroundItemPickup>(PickupClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
		if (Pickup)
		{
			Pickup->InitializeItem(GetRandomLootItem());
		}
	}
}

void ALootManager::SpawnAirDrop(const FVector& DropCenter, float DropRadius)
{
	if (!SupplyDropClass || !HasAuthority()) return;

	FVector2D RandomOffset = FVector2D(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f)).GetSafeNormal() * FMath::FRandRange(0.0f, DropRadius);
	FVector DropOrigin = DropCenter + FVector(RandomOffset.X, RandomOffset.Y, 50000.0f); // 500m in the air

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASupplyDropCrate* Crate = GetWorld()->SpawnActor<ASupplyDropCrate>(SupplyDropClass, DropOrigin, FRotator::ZeroRotator, SpawnParams);
	if (Crate)
	{
		TArray<FLastZoneItem> HighTierLoot;

		FLastZoneItem AWM;
		AWM.ItemID = FName(TEXT("Weapon_AWM"));
		AWM.DisplayName = FText::FromString(TEXT("AWM Sniper Rifle (.300 Magnum)"));
		AWM.Category = EItemCategory::Weapon;
		AWM.Rarity = EItemRarity::Legendary;
		AWM.Quantity = 1;
		HighTierLoot.Add(AWM);

		FLastZoneItem ArmorL3;
		ArmorL3.ItemID = FName(TEXT("Vest_Level3"));
		ArmorL3.DisplayName = FText::FromString(TEXT("Military Heavy Vest (Lv 3)"));
		ArmorL3.Category = EItemCategory::Armor;
		ArmorL3.Rarity = EItemRarity::Legendary;
		ArmorL3.Quantity = 1;
		HighTierLoot.Add(ArmorL3);

		FLastZoneItem MedKit;
		MedKit.ItemID = FName(TEXT("MedKit"));
		MedKit.DisplayName = FText::FromString(TEXT("Military Med Kit (100% Heal)"));
		MedKit.Category = EItemCategory::Medical;
		MedKit.Rarity = EItemRarity::Epic;
		MedKit.Quantity = 1;
		HighTierLoot.Add(MedKit);

		Crate->InitializeSupplyDrop(HighTierLoot);
	}
}

