// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZonePlayerState.h"
#include "Net/UnrealNetwork.h"

ALastZonePlayerState::ALastZonePlayerState()
{
	KillCount = 0;
	DamageDealt = 0.0f;
	HeadshotCount = 0;
	TeamID = 0;
	bIsAlive = true;
	FinalPlacement = 0;
	SurvivalTimeSeconds = 0.0f;
}

void ALastZonePlayerState::AddKill()
{
	KillCount++;
	OnRep_Kills();
}

void ALastZonePlayerState::AddDamage(float Damage)
{
	DamageDealt += Damage;
}

void ALastZonePlayerState::SetAliveStatus(bool bAlive)
{
	bIsAlive = bAlive;
	OnRep_AliveStatus();
}

void ALastZonePlayerState::SetPlacement(int32 Placement)
{
	FinalPlacement = Placement;
}

void ALastZonePlayerState::OnRep_Kills()
{
	OnKillsChanged.Broadcast(KillCount);
}

void ALastZonePlayerState::OnRep_AliveStatus()
{
	OnAliveStatusChanged.Broadcast(bIsAlive);
}

void ALastZonePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALastZonePlayerState, KillCount);
	DOREPLIFETIME(ALastZonePlayerState, DamageDealt);
	DOREPLIFETIME(ALastZonePlayerState, HeadshotCount);
	DOREPLIFETIME(ALastZonePlayerState, TeamID);
	DOREPLIFETIME(ALastZonePlayerState, bIsAlive);
	DOREPLIFETIME(ALastZonePlayerState, FinalPlacement);
	DOREPLIFETIME(ALastZonePlayerState, SurvivalTimeSeconds);
}

