// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneAnimInstance.h"
#include "LastZoneCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/HealthComponent.h"
#include "Components/InventoryComponent.h"
#include "LastZone/Weapons/LastZoneWeaponBase.h"

ULastZoneAnimInstance::ULastZoneAnimInstance()
{
	GroundSpeed = 0.0f;
	Direction = 0.0f;
	bIsAccelerating = false;
	bIsInAir = false;
	bIsCrouched = false;
	bIsProne = false;
	bIsSprinting = false;
	bIsAiming = false;
	AimPitch = 0.0f;
	LeanAngle = 0.0f;
	EquippedWeaponType = EWeaponType::AssaultRifle;
	bHasWeaponEquipped = false;
	bIsDead = false;
}

void ULastZoneAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<ALastZoneCharacter>(TryGetPawnOwner());
}

void ULastZoneAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Character)
	{
		Character = Cast<ALastZoneCharacter>(TryGetPawnOwner());
	}
	if (!Character) return;

	// Calculate ground speed
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.0f;
	GroundSpeed = Velocity.Size();

	// Calculate direction angle (-180 to 180)
	FRotator AimRotation = Character->GetBaseAimRotation();
	Direction = CalculateDirection(Velocity, AimRotation);

	// Locomotion checks
	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (MoveComp)
	{
		bIsAccelerating = MoveComp->GetCurrentAcceleration().Size() > 0.0f;
		bIsInAir = MoveComp->IsFalling();
	}

	// Stances
	EPlayerStance Stance = Character->GetPlayerStance();
	bIsCrouched = (Stance == EPlayerStance::Crouching);
	bIsProne = (Stance == EPlayerStance::Prone);

	// Movement state
	bIsSprinting = (Character->GetPlayerMovementState() == EPlayerMovementState::Sprinting);
	bIsAiming = Character->IsAiming();
	AimPitch = Character->GetAimPitch();

	// Health and weapon status
	if (UHealthComponent* HealthComp = Character->GetHealthComponent())
	{
		bIsDead = HealthComp->IsDead();
	}

	if (UInventoryComponent* InvComp = Character->GetInventoryComponent())
	{
		ALastZoneWeaponBase* CurrentWeapon = InvComp->GetCurrentWeapon();
		bHasWeaponEquipped = (CurrentWeapon != nullptr);
		if (CurrentWeapon)
		{
			EquippedWeaponType = CurrentWeapon->GetStats().WeaponType;
		}
	}
}

