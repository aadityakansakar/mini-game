// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LastZone/LastZoneTypes.h"
#include "InventoryComponent.generated.h"

class ALastZoneWeaponBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdatedSignature, UInventoryComponent*, Inventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponEquippedSignature, ALastZoneWeaponBase*, NewWeapon, int32, SlotIndex);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LASTZONE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon, BlueprintReadOnly, Category = "Weapons")
	ALastZoneWeaponBase* CurrentEquippedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponSlots, BlueprintReadOnly, Category = "Weapons")
	TArray<ALastZoneWeaponBase*> EquippedWeaponSlots; // 0: Primary, 1: Secondary, 2: Sidearm, 3: Melee

	UPROPERTY(ReplicatedUsing = OnRep_Items, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FLastZoneItem> StoredItems;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 BackpackLevel; // 0 to 3

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	float BaseCapacity;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapons")
	int32 ActiveSlotIndex;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_WeaponSlots();

	UFUNCTION()
	void OnRep_Items();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Capacity & Weight calculations
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetMaxCapacity() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetCurrentWeight() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasSpaceFor(const FLastZoneItem& Item) const;

	// Item operations
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	bool AddItem(const FLastZoneItem& NewItem);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	bool RemoveItem(FName ItemID, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemCount(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetAmmoCount(EAmmoType AmmoType) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	bool ConsumeAmmo(EAmmoType AmmoType, int32 Amount);

	// Weapon Slot management
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Weapons")
	bool EquipWeaponToSlot(ALastZoneWeaponBase* Weapon, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Weapons")
	void SwitchToSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Weapons")
	void HolsterWeapon();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Weapons")
	void ServerSwitchSlot(int32 SlotIndex);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory")
	void ServerDropItem(FName ItemID, int32 Quantity);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory")
	void ServerUseConsumable(FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	ALastZoneWeaponBase* GetCurrentWeapon() const { return CurrentEquippedWeapon; }

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	ALastZoneWeaponBase* GetWeaponInSlot(int32 Slot) const { return EquippedWeaponSlots.IsValidIndex(Slot) ? EquippedWeaponSlots[Slot] : nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInventoryUpdatedSignature OnInventoryUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponEquippedSignature OnWeaponEquipped;
};

