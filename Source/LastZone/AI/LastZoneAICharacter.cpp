// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneAICharacter.h"
#include "LastZoneAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "LastZone/Weapons/LastZoneWeaponBase.h"
#include "LastZone/Character/Components/InventoryComponent.h"

ALastZoneAICharacter::ALastZoneAICharacter()
{
	AIControllerClass = ALastZoneAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	BotAimAccuracy = 0.75f;
	BurstDuration = 1.2f;
	DelayBetweenBursts = 0.8f;
}

void ALastZoneAICharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ALastZoneAICharacter::BotFireAtTarget(AActor* TargetActor)
{
	if (!TargetActor) return;

	// Aim rotation towards target with accuracy offset
	FVector StartLoc = GetActorLocation();
	FVector TargetLoc = TargetActor->GetActorLocation() + FVector(0.0f, 0.0f, 40.0f); // Aim at chest

	// Add inaccuracy based on BotAimAccuracy
	float InaccuracySpread = (1.0f - BotAimAccuracy) * 80.0f;
	TargetLoc += FVector(FMath::FRandRange(-InaccuracySpread, InaccuracySpread), FMath::FRandRange(-InaccuracySpread, InaccuracySpread), FMath::FRandRange(-InaccuracySpread * 0.5f, InaccuracySpread * 0.5f));

	FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(StartLoc, TargetLoc);
	SetActorRotation(FRotator(0.0f, LookRot.Yaw, 0.0f));

	if (Controller)
	{
		Controller->SetControlRotation(LookRot);
	}

	StartAim();
	StartFire();
}

void ALastZoneAICharacter::BotStopFiring()
{
	StopFire();
	StopAim();
}

