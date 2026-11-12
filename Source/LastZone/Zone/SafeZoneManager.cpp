// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "SafeZoneManager.h"
#include "SafeZoneVisualizer.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "LastZone/Character/LastZoneCharacter.h"
#include "LastZone/Character/Components/HealthComponent.h"
#include "LastZone/LastZone.h"

ASafeZoneManager::ASafeZoneManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	CurrentCenter = FVector2D::ZeroVector;
	CurrentRadius = 3500.0f; // 3500 meters initial radius
	TargetCenter = FVector2D::ZeroVector;
	TargetRadius = 2200.0f;
	CurrentPhaseIndex = 0;
	PhaseTimer = 180.0f;
	bIsShrinking = false;

	PreviousCenter = CurrentCenter;
	PreviousRadius = CurrentRadius;
	ShrinkElapsedTime = 0.0f;

	// Populate standard Battle Royale safe zone phases
	FSafeZonePhase Phase1;
	Phase1.PhaseNumber = 1;
	Phase1.WaitDurationSeconds = 180.0f;
	Phase1.ShrinkDurationSeconds = 120.0f;
	Phase1.TargetRadiusMeters = 2200.0f;
	Phase1.DamagePerSecond = 1.0f;
	ZonePhases.Add(Phase1);

	FSafeZonePhase Phase2;
	Phase2.PhaseNumber = 2;
	Phase2.WaitDurationSeconds = 120.0f;
	Phase2.ShrinkDurationSeconds = 90.0f;
	Phase2.TargetRadiusMeters = 1400.0f;
	Phase2.DamagePerSecond = 2.0f;
	ZonePhases.Add(Phase2);

	FSafeZonePhase Phase3;
	Phase3.PhaseNumber = 3;
	Phase3.WaitDurationSeconds = 90.0f;
	Phase3.ShrinkDurationSeconds = 60.0f;
	Phase3.TargetRadiusMeters = 800.0f;
	Phase3.DamagePerSecond = 4.0f;
	ZonePhases.Add(Phase3);

	FSafeZonePhase Phase4;
	Phase4.PhaseNumber = 4;
	Phase4.WaitDurationSeconds = 60.0f;
	Phase4.ShrinkDurationSeconds = 45.0f;
	Phase4.TargetRadiusMeters = 400.0f;
	Phase4.DamagePerSecond = 7.0f;
	ZonePhases.Add(Phase4);

	FSafeZonePhase Phase5;
	Phase5.PhaseNumber = 5;
	Phase5.WaitDurationSeconds = 45.0f;
	Phase5.ShrinkDurationSeconds = 30.0f;
	Phase5.TargetRadiusMeters = 150.0f;
	Phase5.DamagePerSecond = 11.0f;
	ZonePhases.Add(Phase5);

	FSafeZonePhase Phase6;
	Phase6.PhaseNumber = 6;
	Phase6.WaitDurationSeconds = 30.0f;
	Phase6.ShrinkDurationSeconds = 25.0f;
	Phase6.TargetRadiusMeters = 0.0f;
	Phase6.DamagePerSecond = 16.0f;
	ZonePhases.Add(Phase6);
}

void ASafeZoneManager::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// Spawn in-world visualizer actor
		if (VisualizerClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			VisualizerInstance = GetWorld()->SpawnActor<ASafeZoneVisualizer>(VisualizerClass, FVector(CurrentCenter.X * 100.0f, CurrentCenter.Y * 100.0f, 0.0f), FRotator::ZeroRotator, SpawnParams);
			if (VisualizerInstance)
			{
				VisualizerInstance->UpdateVisuals(CurrentCenter, CurrentRadius);
			}
		}

		AdvanceToNextPhase();
	}
}

void ASafeZoneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		DamagePlayersOutsideZone(DeltaTime);

		if (!bIsShrinking)
		{
			// Count down waiting phase
			PhaseTimer -= DeltaTime;
			if (PhaseTimer <= 0.0f)
			{
				StartShrinking();
			}
		}
		else
		{
			// Actively shrinking
			if (ZonePhases.IsValidIndex(CurrentPhaseIndex))
			{
				float ShrinkDuration = ZonePhases[CurrentPhaseIndex].ShrinkDurationSeconds;
				ShrinkElapsedTime += DeltaTime;
				float Alpha = FMath::Clamp(ShrinkElapsedTime / ShrinkDuration, 0.0f, 1.0f);

				CurrentCenter = FMath::Lerp(PreviousCenter, TargetCenter, Alpha);
				CurrentRadius = FMath::Lerp(PreviousRadius, TargetRadius, Alpha);

				if (VisualizerInstance)
				{
					VisualizerInstance->UpdateVisuals(CurrentCenter, CurrentRadius);
				}

				if (Alpha >= 1.0f)
				{
					bIsShrinking = false;
					CurrentPhaseIndex++;
					AdvanceToNextPhase();
				}
			}
		}
	}
}

void ASafeZoneManager::AdvanceToNextPhase()
{
	if (!ZonePhases.IsValidIndex(CurrentPhaseIndex))
	{
		return; // Reached final collapse
	}

	const FSafeZonePhase& Phase = ZonePhases[CurrentPhaseIndex];
	PhaseTimer = Phase.WaitDurationSeconds;
	bIsShrinking = false;

	PreviousCenter = CurrentCenter;
	PreviousRadius = CurrentRadius;
	TargetRadius = Phase.TargetRadiusMeters;

	// Pick random TargetCenter strictly inside CurrentRadius such that TargetCircle fits inside CurrentCircle
	float MaxOffset = FMath::Max(0.0f, CurrentRadius - TargetRadius);
	FVector2D RandomDirection = FVector2D(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f)).GetSafeNormal();
	float RandomDist = FMath::FRandRange(0.0f, MaxOffset);

	TargetCenter = CurrentCenter + (RandomDirection * RandomDist);

	OnZonePhaseChanged.Broadcast(CurrentPhaseIndex + 1, TargetRadius, PhaseTimer);
}

void ASafeZoneManager::StartShrinking()
{
	bIsShrinking = true;
	ShrinkElapsedTime = 0.0f;
	PreviousCenter = CurrentCenter;
	PreviousRadius = CurrentRadius;

	if (ZonePhases.IsValidIndex(CurrentPhaseIndex))
	{
		TargetRadius = ZonePhases[CurrentPhaseIndex].TargetRadiusMeters;
	}
}

void ASafeZoneManager::DamagePlayersOutsideZone(float DeltaTime)
{
	if (!ZonePhases.IsValidIndex(CurrentPhaseIndex)) return;

	float DPS = ZonePhases[CurrentPhaseIndex].DamagePerSecond;
	float RadiusCm = CurrentRadius * 100.0f;
	FVector2D CenterCm = CurrentCenter * 100.0f;

	for (TActorIterator<ALastZoneCharacter> It(GetWorld()); It; ++It)
	{
		ALastZoneCharacter* Character = *It;
		if (Character && Character->GetHealthComponent() && !Character->GetHealthComponent()->IsDead())
		{
			FVector CharLoc = Character->GetActorLocation();
			FVector2D CharLoc2D(CharLoc.X, CharLoc.Y);
			float Dist = FVector2D::Distance(CharLoc2D, CenterCm);

			if (Dist > RadiusCm)
			{
				float DamageToApply = DPS * DeltaTime;
				UGameplayStatics::ApplyDamage(Character, DamageToApply, nullptr, this, UDamageType::StaticClass());
			}
		}
	}
}

bool ASafeZoneManager::IsInsideCurrentZone(const FVector& WorldLocation) const
{
	FVector2D Loc2D(WorldLocation.X, WorldLocation.Y);
	float DistCm = FVector2D::Distance(Loc2D, CurrentCenter * 100.0f);
	return DistCm <= (CurrentRadius * 100.0f);
}

bool ASafeZoneManager::IsInsideTargetZone(const FVector& WorldLocation) const
{
	FVector2D Loc2D(WorldLocation.X, WorldLocation.Y);
	float DistCm = FVector2D::Distance(Loc2D, TargetCenter * 100.0f);
	return DistCm <= (TargetRadius * 100.0f);
}

float ASafeZoneManager::GetDistanceToSafeZoneEdge(const FVector& WorldLocation) const
{
	FVector2D Loc2D(WorldLocation.X, WorldLocation.Y);
	float DistCm = FVector2D::Distance(Loc2D, CurrentCenter * 100.0f);
	float RadiusCm = CurrentRadius * 100.0f;
	return (RadiusCm - DistCm) * 0.01f; // Returns positive if inside, negative if outside (in meters)
}

void ASafeZoneManager::OnRep_CurrentCenter()
{
	if (VisualizerInstance)
	{
		VisualizerInstance->UpdateVisuals(CurrentCenter, CurrentRadius);
	}
}

void ASafeZoneManager::OnRep_CurrentRadius()
{
	if (VisualizerInstance)
	{
		VisualizerInstance->UpdateVisuals(CurrentCenter, CurrentRadius);
	}
}

void ASafeZoneManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASafeZoneManager, CurrentCenter);
	DOREPLIFETIME(ASafeZoneManager, CurrentRadius);
	DOREPLIFETIME(ASafeZoneManager, TargetCenter);
	DOREPLIFETIME(ASafeZoneManager, TargetRadius);
	DOREPLIFETIME(ASafeZoneManager, CurrentPhaseIndex);
	DOREPLIFETIME(ASafeZoneManager, PhaseTimer);
	DOREPLIFETIME(ASafeZoneManager, bIsShrinking);
}

