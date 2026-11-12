// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "ArmorComponent.h"
#include "Net/UnrealNetwork.h"

UArmorComponent::UArmorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	VestLevel = 0;
	VestDurability = 0.0f;
	MaxVestDurability = 200.0f;

	HelmetLevel = 0;
	HelmetDurability = 0.0f;
	MaxHelmetDurability = 150.0f;
}

void UArmorComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UArmorComponent::AbsorbDamage(float IncomingDamage, const class UDamageType* DamageType)
{
	if (IncomingDamage <= 0.0f)
	{
		return 0.0f;
	}

	float MitigatedDamage = IncomingDamage;

	// Check vest mitigation if vest is equipped and has durability
	if (VestLevel > 0 && VestDurability > 0.0f)
	{
		float DamageReductionRatio = 0.30f;
		if (VestLevel == 2)
		{
			DamageReductionRatio = 0.40f;
		}
		else if (VestLevel >= 3)
		{
			DamageReductionRatio = 0.55f;
		}

		float AbsorbedAmount = IncomingDamage * DamageReductionRatio;
		VestDurability = FMath::Clamp(VestDurability - IncomingDamage, 0.0f, MaxVestDurability);

		if (VestDurability <= 0.0f)
		{
			VestLevel = 0;
		}

		MitigatedDamage = IncomingDamage - AbsorbedAmount;
		OnArmorChanged.Broadcast(VestDurability, HelmetDurability, VestLevel);
	}

	return MitigatedDamage;
}

void UArmorComponent::EquipVest(int32 Level)
{
	VestLevel = FMath::Clamp(Level, 0, 3);
	switch (VestLevel)
	{
	case 1:
		MaxVestDurability = 200.0f;
		break;
	case 2:
		MaxVestDurability = 220.0f;
		break;
	case 3:
		MaxVestDurability = 250.0f;
		break;
	default:
		MaxVestDurability = 0.0f;
		break;
	}
	VestDurability = MaxVestDurability;
	OnArmorChanged.Broadcast(VestDurability, HelmetDurability, VestLevel);
}

void UArmorComponent::EquipHelmet(int32 Level)
{
	HelmetLevel = FMath::Clamp(Level, 0, 3);
	switch (HelmetLevel)
	{
	case 1:
		MaxHelmetDurability = 80.0f;
		break;
	case 2:
		MaxHelmetDurability = 150.0f;
		break;
	case 3:
		MaxHelmetDurability = 230.0f;
		break;
	default:
		MaxHelmetDurability = 0.0f;
		break;
	}
	HelmetDurability = MaxHelmetDurability;
	OnArmorChanged.Broadcast(VestDurability, HelmetDurability, VestLevel);
}

void UArmorComponent::OnRep_Vest()
{
	OnArmorChanged.Broadcast(VestDurability, HelmetDurability, VestLevel);
}

void UArmorComponent::OnRep_Helmet()
{
	OnArmorChanged.Broadcast(VestDurability, HelmetDurability, VestLevel);
}

void UArmorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UArmorComponent, VestLevel);
	DOREPLIFETIME(UArmorComponent, VestDurability);
	DOREPLIFETIME(UArmorComponent, HelmetLevel);
	DOREPLIFETIME(UArmorComponent, HelmetDurability);
}

