// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "LastZone/Weapons/LastZoneWeaponBase.h"
#include "LastZone/Loot/GroundItemPickup.h"
#include "HealthComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	BaseCapacity = 150.0f;
	BackpackLevel = 0;
	ActiveSlotIndex = -1;
	CurrentEquippedWeapon = nullptr;

	EquippedWeaponSlots.SetNumZeroed(4); // 0: Primary, 1: Secondary, 2: Sidearm, 3: Melee
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UInventoryComponent::GetMaxCapacity() const
{
	switch (BackpackLevel)
	{
	case 1:  return 200.0f;
	case 2:  return 260.0f;
	case 3:  return 340.0f;
	default: return BaseCapacity;
	}
}

float UInventoryComponent::GetCurrentWeight() const
{
	float TotalWeight = 0.0f;
	for (const FLastZoneItem& Item : StoredItems)
	{
		TotalWeight += Item.Weight * Item.Quantity;
	}
	return TotalWeight;
}

bool UInventoryComponent::HasSpaceFor(const FLastZoneItem& Item) const
{
	return (GetCurrentWeight() + (Item.Weight * Item.Quantity)) <= GetMaxCapacity();
}

bool UInventoryComponent::AddItem(const FLastZoneItem& NewItem)
{
	if (!HasSpaceFor(NewItem))
	{
		return false;
	}

	// Try to stack with existing item
	for (FLastZoneItem& ExistingItem : StoredItems)
	{
		if (ExistingItem.ItemID == NewItem.ItemID && ExistingItem.Quantity < ExistingItem.MaxStack)
		{
			int32 SpaceLeft = ExistingItem.MaxStack - ExistingItem.Quantity;
			int32 AddedAmount = FMath::Min(SpaceLeft, NewItem.Quantity);
			ExistingItem.Quantity += AddedAmount;

			int32 RemainingAmount = NewItem.Quantity - AddedAmount;
			if (RemainingAmount <= 0)
			{
				OnInventoryUpdated.Broadcast(this);
				return true;
			}
		}
	}

	StoredItems.Add(NewItem);
	OnInventoryUpdated.Broadcast(this);
	return true;
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
	for (int32 i = StoredItems.Num() - 1; i >= 0; --i)
	{
		if (StoredItems[i].ItemID == ItemID)
		{
			if (StoredItems[i].Quantity > Quantity)
			{
				StoredItems[i].Quantity -= Quantity;
				OnInventoryUpdated.Broadcast(this);
				return true;
			}
			else
			{
				Quantity -= StoredItems[i].Quantity;
				StoredItems.RemoveAt(i);
				if (Quantity <= 0)
				{
					OnInventoryUpdated.Broadcast(this);
					return true;
				}
			}
		}
	}
	return false;
}

int32 UInventoryComponent::GetItemCount(FName ItemID) const
{
	int32 Count = 0;
	for (const FLastZoneItem& Item : StoredItems)
	{
		if (Item.ItemID == ItemID)
		{
			Count += Item.Quantity;
		}
	}
	return Count;
}

int32 UInventoryComponent::GetAmmoCount(EAmmoType AmmoType) const
{
	FName ExpectedID;
	switch (AmmoType)
	{
	case EAmmoType::Ammo_556mm:   ExpectedID = FName(TEXT("Ammo_556")); break;
	case EAmmoType::Ammo_762mm:   ExpectedID = FName(TEXT("Ammo_762")); break;
	case EAmmoType::Ammo_9mm:     ExpectedID = FName(TEXT("Ammo_9mm")); break;
	case EAmmoType::Ammo_12Gauge: ExpectedID = FName(TEXT("Ammo_12Gauge")); break;
	case EAmmoType::Ammo_300Mag:  ExpectedID = FName(TEXT("Ammo_300Mag")); break;
	default: return 0;
	}
	return GetItemCount(ExpectedID);
}

bool UInventoryComponent::ConsumeAmmo(EAmmoType AmmoType, int32 Amount)
{
	FName ExpectedID;
	switch (AmmoType)
	{
	case EAmmoType::Ammo_556mm:   ExpectedID = FName(TEXT("Ammo_556")); break;
	case EAmmoType::Ammo_762mm:   ExpectedID = FName(TEXT("Ammo_762")); break;
	case EAmmoType::Ammo_9mm:     ExpectedID = FName(TEXT("Ammo_9mm")); break;
	case EAmmoType::Ammo_12Gauge: ExpectedID = FName(TEXT("Ammo_12Gauge")); break;
	case EAmmoType::Ammo_300Mag:  ExpectedID = FName(TEXT("Ammo_300Mag")); break;
	default: return false;
	}
	return RemoveItem(ExpectedID, Amount);
}

bool UInventoryComponent::EquipWeaponToSlot(ALastZoneWeaponBase* Weapon, int32 SlotIndex)
{
	if (!Weapon || !EquippedWeaponSlots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	// Unholster or drop old weapon in slot if present
	if (EquippedWeaponSlots[SlotIndex] != nullptr && EquippedWeaponSlots[SlotIndex] != Weapon)
	{
		EquippedWeaponSlots[SlotIndex]->SetActorHiddenInGame(true);
		EquippedWeaponSlots[SlotIndex]->SetActorEnableCollision(false);
	}

	EquippedWeaponSlots[SlotIndex] = Weapon;
	SwitchToSlot(SlotIndex);
	return true;
}

void UInventoryComponent::SwitchToSlot(int32 SlotIndex)
{
	if (!EquippedWeaponSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	ALastZoneWeaponBase* TargetWeapon = EquippedWeaponSlots[SlotIndex];
	if (CurrentEquippedWeapon)
	{
		CurrentEquippedWeapon->SetActorHiddenInGame(true);
	}

	CurrentEquippedWeapon = TargetWeapon;
	ActiveSlotIndex = SlotIndex;

	if (CurrentEquippedWeapon)
	{
		CurrentEquippedWeapon->SetActorHiddenInGame(false);
	}

	OnWeaponEquipped.Broadcast(CurrentEquippedWeapon, ActiveSlotIndex);
}

void UInventoryComponent::HolsterWeapon()
{
	if (CurrentEquippedWeapon)
	{
		CurrentEquippedWeapon->SetActorHiddenInGame(true);
		CurrentEquippedWeapon = nullptr;
		ActiveSlotIndex = -1;
		OnWeaponEquipped.Broadcast(nullptr, -1);
	}
}

void UInventoryComponent::ServerSwitchSlot_Implementation(int32 SlotIndex)
{
	SwitchToSlot(SlotIndex);
}

void UInventoryComponent::ServerDropItem_Implementation(FName ItemID, int32 Quantity)
{
	if (!RemoveItem(ItemID, Quantity))
	{
		return;
	}

	// Spawn ground item in front of player
	if (AActor* OwnerActor = GetOwner())
	{
		FVector SpawnLocation = OwnerActor->GetActorLocation() + (OwnerActor->GetActorForwardVector() * 120.0f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (UWorld* World = GetWorld())
		{
			AGroundItemPickup* Pickup = World->SpawnActor<AGroundItemPickup>(AGroundItemPickup::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			if (Pickup)
			{
				FLastZoneItem DroppedItem;
				DroppedItem.ItemID = ItemID;
				DroppedItem.Quantity = Quantity;
				Pickup->InitializeItem(DroppedItem);
			}
		}
	}
}

void UInventoryComponent::ServerUseConsumable_Implementation(FName ItemID)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	UHealthComponent* HealthComp = OwnerActor->FindComponentByClass<UHealthComponent>();
	if (!HealthComp) return;

	// Process Medical / Boost item effect
	if (ItemID == FName(TEXT("FirstAidKit")))
	{
		if (HealthComp->GetHealth() < 75.0f && RemoveItem(ItemID, 1))
		{
			HealthComp->Heal(75.0f - HealthComp->GetHealth());
		}
	}
	else if (ItemID == FName(TEXT("MedKit")))
	{
		if (HealthComp->GetHealth() < 100.0f && RemoveItem(ItemID, 1))
		{
			HealthComp->Heal(100.0f);
		}
	}
	else if (ItemID == FName(TEXT("EnergyDrink")))
	{
		if (RemoveItem(ItemID, 1))
		{
			HealthComp->AddBoost(40.0f);
		}
	}
	else if (ItemID == FName(TEXT("Painkillers")))
	{
		if (RemoveItem(ItemID, 1))
		{
			HealthComp->AddBoost(60.0f);
		}
	}
	else if (ItemID == FName(TEXT("Bandage")))
	{
		if (HealthComp->GetHealth() < 75.0f && RemoveItem(ItemID, 1))
		{
			HealthComp->Heal(10.0f);
		}
	}
}

void UInventoryComponent::OnRep_EquippedWeapon()
{
	OnWeaponEquipped.Broadcast(CurrentEquippedWeapon, ActiveSlotIndex);
}

void UInventoryComponent::OnRep_WeaponSlots()
{
	// Client sync weapon slots
}

void UInventoryComponent::OnRep_Items()
{
	OnInventoryUpdated.Broadcast(this);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, CurrentEquippedWeapon);
	DOREPLIFETIME(UInventoryComponent, EquippedWeaponSlots);
	DOREPLIFETIME(UInventoryComponent, StoredItems);
	DOREPLIFETIME(UInventoryComponent, BackpackLevel);
	DOREPLIFETIME(UInventoryComponent, ActiveSlotIndex);
}

