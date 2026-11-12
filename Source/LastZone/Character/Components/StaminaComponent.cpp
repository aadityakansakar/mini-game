// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "StaminaComponent.h"
#include "Net/UnrealNetwork.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	MaxStamina = 100.0f;
	CurrentStamina = MaxStamina;
	SprintDrainRate = 12.0f;
	RecoveryRate = 18.0f;
	JumpStaminaCost = 15.0f;
	VaultStaminaCost = 10.0f;
	bIsExhausted = false;
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentStamina = MaxStamina;
}

void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		// Passive recovery when not exhausted or regenerating
		if (CurrentStamina < MaxStamina)
		{
			float Recovery = RecoveryRate * DeltaTime;
			CurrentStamina = FMath::Clamp(CurrentStamina + Recovery, 0.0f, MaxStamina);

			if (bIsExhausted && CurrentStamina >= 25.0f)
			{
				bIsExhausted = false;
				OnStaminaExhausted.Broadcast(false);
			}

			OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
		}
	}
}

bool UStaminaComponent::ConsumeStamina(float Amount)
{
	if (Amount <= 0.0f) return true;

	if (CurrentStamina >= Amount)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.0f, MaxStamina);
		if (CurrentStamina <= 0.0f)
		{
			bIsExhausted = true;
			OnStaminaExhausted.Broadcast(true);
		}
		OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
		return true;
	}
	return false;
}

void UStaminaComponent::ConsumeSprint(float DeltaTime)
{
	float Drain = SprintDrainRate * DeltaTime;
	CurrentStamina = FMath::Clamp(CurrentStamina - Drain, 0.0f, MaxStamina);

	if (CurrentStamina <= 0.0f)
	{
		bIsExhausted = true;
		OnStaminaExhausted.Broadcast(true);
	}
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UStaminaComponent::OnRep_Stamina()
{
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UStaminaComponent::OnRep_Exhausted()
{
	OnStaminaExhausted.Broadcast(bIsExhausted);
}

void UStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStaminaComponent, CurrentStamina);
	DOREPLIFETIME(UStaminaComponent, bIsExhausted);
}

