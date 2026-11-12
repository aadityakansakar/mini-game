// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneGameState.h"
#include "Net/UnrealNetwork.h"

ALastZoneGameState::ALastZoneGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ZoneMatchState = ELastZoneMatchState::WaitingForPlayers;
	AlivePlayersCount = 0;
	TotalPlayersCount = 0;
	MatchElapsedTime = 0.0f;
	SafeZoneManager = nullptr;
}

void ALastZoneGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (ZoneMatchState == ELastZoneMatchState::ActiveBattle || ZoneMatchState == ELastZoneMatchState::ZoneClosing)
		{
			MatchElapsedTime += DeltaTime;
		}
	}
}

void ALastZoneGameState::SetZoneMatchState(ELastZoneMatchState NewState)
{
	ZoneMatchState = NewState;
	OnRep_ZoneMatchState();
}

void ALastZoneGameState::SetAlivePlayersCount(int32 NewCount)
{
	AlivePlayersCount = NewCount;
	OnRep_AlivePlayers();
}

void ALastZoneGameState::SetTotalPlayersCount(int32 Total)
{
	TotalPlayersCount = Total;
	OnRep_AlivePlayers();
}

void ALastZoneGameState::MulticastBroadcastKillFeed_Implementation(const FKillFeedEntry& Entry)
{
	OnKillFeedBroadcast.Broadcast(Entry);
}

void ALastZoneGameState::OnRep_ZoneMatchState()
{
	OnMatchStateChanged.Broadcast(ZoneMatchState);
}

void ALastZoneGameState::OnRep_AlivePlayers()
{
	OnPlayerCountChanged.Broadcast(AlivePlayersCount, TotalPlayersCount);
}

void ALastZoneGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALastZoneGameState, ZoneMatchState);
	DOREPLIFETIME(ALastZoneGameState, AlivePlayersCount);
	DOREPLIFETIME(ALastZoneGameState, TotalPlayersCount);
	DOREPLIFETIME(ALastZoneGameState, MatchElapsedTime);
	DOREPLIFETIME(ALastZoneGameState, SafeZoneManager);
}

