// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArmorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnArmorChangedSignature, float, VestDurability, float, HelmetDurability, int32, VestLevel);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LASTZONE_API UArmorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UArmorComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Vest, EditAnywhere, BlueprintReadWrite, Category = "Armor")
	int32 VestLevel; // 0 = None, 1 = Police Vest, 2 = Military Vest, 3 = Heavy Ceramic Vest

	UPROPERTY(ReplicatedUsing = OnRep_Vest, EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float VestDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float MaxVestDurability;

	UPROPERTY(ReplicatedUsing = OnRep_Helmet, EditAnywhere, BlueprintReadWrite, Category = "Armor")
	int32 HelmetLevel; // 0 = None, 1 = Motorcycle Helmet, 2 = Military Helmet, 3 = Spetsnaz Helmet

	UPROPERTY(ReplicatedUsing = OnRep_Helmet, EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float HelmetDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float MaxHelmetDurability;

	UFUNCTION()
	void OnRep_Vest();

	UFUNCTION()
	void OnRep_Helmet();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Computes mitigated damage and reduces armor durability
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Armor")
	float AbsorbDamage(float IncomingDamage, const class UDamageType* DamageType);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Armor")
	void EquipVest(int32 Level);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Armor")
	void EquipHelmet(int32 Level);

	UFUNCTION(BlueprintCallable, Category = "Armor")
	float GetVestPercent() const { return MaxVestDurability > 0.0f ? VestDurability / MaxVestDurability : 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Armor")
	float GetHelmetPercent() const { return MaxHelmetDurability > 0.0f ? HelmetDurability / MaxHelmetDurability : 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Armor")
	int32 GetVestLevel() const { return VestLevel; }

	UFUNCTION(BlueprintCallable, Category = "Armor")
	int32 GetHelmetLevel() const { return HelmetLevel; }

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnArmorChangedSignature OnArmorChanged;
};

