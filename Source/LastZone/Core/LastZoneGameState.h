// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LastZone/LastZoneTypes.h"
#include "LastZoneGameState.generated.h"

class ASafeZoneManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStateChangedSignature, ELastZoneMatchState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerCountChangedSignature, int32, AliveCount, int32, TotalCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillFeedBroadcastSignature, const FKillFeedEntry&, Entry);

UCLASS()
class LASTZONE_API ALastZoneGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ALastZoneGameState();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_ZoneMatchState, BlueprintReadOnly, Category = "Match")
	ELastZoneMatchState ZoneMatchState;

	UPROPERTY(ReplicatedUsing = OnRep_AlivePlayers, BlueprintReadOnly, Category = "Match")
	int32 AlivePlayersCount;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Match")
	int32 TotalPlayersCount;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Match")
	float MatchElapsedTime;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Match")
	ASafeZoneManager* SafeZoneManager;

	UFUNCTION()
	void OnRep_ZoneMatchState();

	UFUNCTION()
	void OnRep_AlivePlayers();

public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Match")
	void SetZoneMatchState(ELastZoneMatchState NewState);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Match")
	void SetAlivePlayersCount(int32 NewCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Match")
	void SetTotalPlayersCount(int32 Total);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Match")
	void SetSafeZoneManager(ASafeZoneManager* Manager) { SafeZoneManager = Manager; }

	UFUNCTION(NetMulticast, Reliable, Category = "Match")
	void MulticastBroadcastKillFeed(const FKillFeedEntry& Entry);

	UFUNCTION(BlueprintCallable, Category = "Match")
	ELastZoneMatchState GetZoneMatchState() const { return ZoneMatchState; }

	UFUNCTION(BlueprintCallable, Category = "Match")
	int32 GetAlivePlayers() const { return AlivePlayersCount; }

	UFUNCTION(BlueprintCallable, Category = "Match")
	int32 GetTotalPlayers() const { return TotalPlayersCount; }

	UFUNCTION(BlueprintCallable, Category = "Match")
	float GetElapsedTime() const { return MatchElapsedTime; }

	UFUNCTION(BlueprintCallable, Category = "Match")
	ASafeZoneManager* GetSafeZoneManager() const { return SafeZoneManager; }

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMatchStateChangedSignature OnMatchStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerCountChangedSignature OnPlayerCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnKillFeedBroadcastSignature OnKillFeedBroadcast;
};

