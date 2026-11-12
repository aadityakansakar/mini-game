// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LastZone/LastZoneTypes.h"
#include "SafeZoneManager.generated.h"

class ASafeZoneVisualizer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnZonePhaseChangedSignature, int32, PhaseIndex, float, NextRadius, float, TimeRemaining);

UCLASS()
class LASTZONE_API ASafeZoneManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASafeZoneManager();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Zone|Phases")
	TArray<FSafeZonePhase> ZonePhases;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentCenter, BlueprintReadOnly, Category = "Zone|State")
	FVector2D CurrentCenter;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentRadius, BlueprintReadOnly, Category = "Zone|State")
	float CurrentRadius;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zone|State")
	FVector2D TargetCenter;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zone|State")
	float TargetRadius;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zone|State")
	int32 CurrentPhaseIndex;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zone|State")
	float PhaseTimer;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zone|State")
	bool bIsShrinking;

	UPROPERTY(EditDefaultsOnly, Category = "Zone|Visualizer")
	TSubclassOf<ASafeZoneVisualizer> VisualizerClass;

	UPROPERTY()
	ASafeZoneVisualizer* VisualizerInstance;

	FVector2D PreviousCenter;
	float PreviousRadius;
	float ShrinkElapsedTime;

	UFUNCTION()
	void OnRep_CurrentCenter();

	UFUNCTION()
	void OnRep_CurrentRadius();

	void AdvanceToNextPhase();
	void StartShrinking();
	void DamagePlayersOutsideZone(float DeltaTime);

public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Checks if a world position is inside the current safe zone
	UFUNCTION(BlueprintCallable, Category = "Zone")
	bool IsInsideCurrentZone(const FVector& WorldLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Zone")
	bool IsInsideTargetZone(const FVector& WorldLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Zone")
	float GetDistanceToSafeZoneEdge(const FVector& WorldLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Zone")
	FVector2D GetCurrentCenter() const { return CurrentCenter; }

	UFUNCTION(BlueprintCallable, Category = "Zone")
	float GetCurrentRadius() const { return CurrentRadius; }

	UFUNCTION(BlueprintCallable, Category = "Zone")
	FVector2D GetTargetCenter() const { return TargetCenter; }

	UFUNCTION(BlueprintCallable, Category = "Zone")
	float GetTargetRadius() const { return TargetRadius; }

	UFUNCTION(BlueprintCallable, Category = "Zone")
	float GetTimeRemainingInPhase() const { return PhaseTimer; }

	UFUNCTION(BlueprintCallable, Category = "Zone")
	bool IsShrinking() const { return bIsShrinking; }

	UPROPERTY(BlueprintAssignable, Category = "Zone|Events")
	FOnZonePhaseChangedSignature OnZonePhaseChanged;
};

