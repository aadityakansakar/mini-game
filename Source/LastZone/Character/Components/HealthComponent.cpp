// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Engine/DamageEvents.h"
#include "ArmorComponent.h"
#include "LastZone/LastZone.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	CurrentBoost = 0.0f;
	bIsDowned = false;
	bIsDead = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
		CurrentHealth = MaxHealth;
	}
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		// Boost bar logic: boost decays slowly, healing character over time
		if (CurrentBoost > 0.0f)
		{
			float BoostDecay = 1.0f * DeltaTime; // 1 boost per second decay
			CurrentBoost = FMath::Clamp(CurrentBoost - BoostDecay, 0.0f, 100.0f);

			// If health is below max, heal from boost
			if (CurrentHealth < MaxHealth && !bIsDead && !bIsDowned)
			{
				float HealRate = (CurrentBoost > 50.0f) ? 2.5f : 1.2f; // Higher boost heals faster
				Heal(HealRate * DeltaTime);
			}
		}

		// Bleedout when downed
		if (bIsDowned && !bIsDead)
		{
			CurrentHealth -= 1.5f * DeltaTime;
			if (CurrentHealth <= 0.0f)
			{
				bIsDead = true;
				CurrentHealth = 0.0f;
				OnDeath.Broadcast(nullptr, GetOwner());
			}
		}
	}
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	float ActualDamage = Damage;

	// Check if owner has ArmorComponent to mitigate damage
	if (AActor* OwnerActor = GetOwner())
	{
		if (UArmorComponent* ArmorComp = OwnerActor->FindComponentByClass<UArmorComponent>())
		{
			ActualDamage = ArmorComp->AbsorbDamage(Damage, DamageType);
		}
	}

	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(this, CurrentHealth, -ActualDamage, DamageType);

	if (CurrentHealth <= 0.0f)
	{
		if (!bIsDowned)
		{
			// Enter downed state first in team modes, or die instantly
			bIsDowned = true;
			CurrentHealth = 100.0f; // Downed health pool
			OnDowned.Broadcast(GetOwner());
		}
		else
		{
			bIsDead = true;
			OnDeath.Broadcast(InstigatedBy ? InstigatedBy->GetPawn() : nullptr, GetOwner());
		}
	}
}

void UHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || bIsDead)
	{
		return;
	}

	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	float Delta = CurrentHealth - OldHealth;

	if (Delta > 0.0f)
	{
		OnHealthChanged.Broadcast(this, CurrentHealth, Delta, nullptr);
	}
}

void UHealthComponent::AddBoost(float BoostAmount)
{
	if (BoostAmount <= 0.0f || bIsDead)
	{
		return;
	}

	CurrentBoost = FMath::Clamp(CurrentBoost + BoostAmount, 0.0f, 100.0f);
}

void UHealthComponent::Revive()
{
	if (bIsDowned && !bIsDead)
	{
		bIsDowned = false;
		CurrentHealth = 25.0f; // Revive with 25 health
		OnRevived.Broadcast(GetOwner());
	}
}

void UHealthComponent::OnRep_Health(float OldHealth)
{
	float Delta = CurrentHealth - OldHealth;
	OnHealthChanged.Broadcast(this, CurrentHealth, Delta, nullptr);
}

void UHealthComponent::OnRep_Boost(float OldBoost)
{
	// Client boost update
}

void UHealthComponent::OnRep_IsDowned()
{
	if (bIsDowned)
	{
		OnDowned.Broadcast(GetOwner());
	}
}

void UHealthComponent::OnRep_IsDead()
{
	if (bIsDead)
	{
		OnDeath.Broadcast(nullptr, GetOwner());
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
	DOREPLIFETIME(UHealthComponent, CurrentBoost);
	DOREPLIFETIME(UHealthComponent, bIsDowned);
	DOREPLIFETIME(UHealthComponent, bIsDead);
}

