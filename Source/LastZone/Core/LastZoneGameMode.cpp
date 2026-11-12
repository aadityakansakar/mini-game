// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneGameMode.h"
#include "LastZoneGameState.h"
#include "LastZonePlayerState.h"
#include "LastZonePlayerController.h"
#include "LastZone/Character/LastZoneCharacter.h"
#include "LastZone/AI/LastZoneAICharacter.h"
#include "LastZone/Zone/SafeZoneManager.h"
#include "LastZone/Loot/LootManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

ALastZoneGameMode::ALastZoneGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = ALastZoneCharacter::StaticClass();
	PlayerControllerClass = ALastZonePlayerController::StaticClass();
	GameStateClass = ALastZoneGameState::StaticClass();
	PlayerStateClass = ALastZonePlayerState::StaticClass();

	SafeZoneManagerClass = ASafeZoneManager::StaticClass();
	LootManagerClass = ALootManager::StaticClass();
	BotCharacterClass = ALastZoneAICharacter::StaticClass();

	TargetPlayerCount = 50;
	WarmupDuration = 30.0f;
	CurrentWarmupTimeRemaining = WarmupDuration;
	bEnableBots = true;
	BotCount = 40; // 10 human players + 40 bots = 50 players

	ActiveSafeZoneManager = nullptr;
	ActiveLootManager = nullptr;
}

void ALastZoneGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World) return;

	// Spawn Safe Zone Manager
	if (SafeZoneManagerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActiveSafeZoneManager = World->SpawnActor<ASafeZoneManager>(SafeZoneManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (ALastZoneGameState* ZoneGS = GetGameState<ALastZoneGameState>())
		{
			ZoneGS->SetSafeZoneManager(ActiveSafeZoneManager);
		}
	}

	// Spawn Loot Manager
	if (LootManagerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActiveLootManager = World->SpawnActor<ALootManager>(LootManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}

	StartWarmup();
}

void ALastZoneGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ALastZoneGameState* ZoneGS = GetGameState<ALastZoneGameState>())
	{
		ZoneGS->SetTotalPlayersCount(ZoneGS->GetTotalPlayers() + 1);
		ZoneGS->SetAlivePlayersCount(ZoneGS->GetAlivePlayers() + 1);
	}
}

void ALastZoneGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (ALastZoneGameState* ZoneGS = GetGameState<ALastZoneGameState>())
	{
		ZoneGS->SetAlivePlayersCount(FMath::Max(0, ZoneGS->GetAlivePlayers() - 1));
		CheckWinCondition();
	}
}

void ALastZoneGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALastZoneGameMode::StartWarmup()
{
	if (ALastZoneGameState* ZoneGS = GetGameState<ALastZoneGameState>())
	{
		ZoneGS->SetZoneMatchState(ELastZoneMatchState::WarmupCountdown);
	}

	CurrentWarmupTimeRemaining = WarmupDuration;
	GetWorldTimerManager().SetTimer(WarmupTimerHandle, this, &ALastZoneGameMode::WarmupCountdownTick, 1.0f, true);
}

void ALastZoneGameMode::WarmupCountdownTick()
{
	CurrentWarmupTimeRemaining -= 1.0f;

	if (CurrentWarmupTimeRemaining <= 0.0f)
	{
		GetWorldTimerManager().ClearTimer(WarmupTimerHandle);
		StartMatch();
	}
}

void ALastZoneGameMode::StartMatch()
{
	if (ALastZoneGameState* ZoneGS = GetGameState<ALastZoneGameState>())
	{
		ZoneGS->SetZoneMatchState(ELastZoneMatchState::ActiveBattle);
	}

	if (bEnableBots)
	{
		SpawnBots();
	}
}

void ALastZoneGameMode::SpawnBots()
{
	UWorld* World = GetWorld();
	if (!World || !BotCharacterClass) return;

	for (int32 i = 0; i < BotCount; ++i)
	{
		// Spawn bots scattered across the island map
		float Angle = FMath::FRandRange(0.0f, 2.0f * PI);
		float Dist = FMath::FRandRange(50000.0f, 250000.0f); // 500m to 2500m
		FVector SpawnLoc(FMath::Cos(Angle) * Dist, FMath::Sin(Angle) * Dist, 200.0f);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ALastZoneAICharacter* Bot = World->SpawnActor<ALastZoneAICharacter>(BotCharacterClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
		if (Bot)
		{
			if (ALastZonePlayerState* BotPS = Cast<ALastZonePlayerState>(Bot->GetPlayerState()))
			{
				BotPS->SetPlayerName(FString::Printf(TEXT("Soldier_%02d"), i + 1));
				BotPS->SetAliveStatus(true);
			}

			if (ALastZoneGameState* ZoneGS = GetGameState<ALastZoneGameState>())
			{
				ZoneGS->SetTotalPlayersCount(ZoneGS->GetTotalPlayers() + 1);
				ZoneGS->SetAlivePlayersCount(ZoneGS->GetAlivePlayers() + 1);
			}
		}
	}
}

void ALastZoneGameMode::OnCharacterEliminated(AController* Killer, AController* Victim, AActor* DamageCauser)
{
	if (!Victim) return;

	ALastZonePlayerState* VictimPS = Cast<ALastZonePlayerState>(Victim->PlayerState);
	if (VictimPS)
	{
		VictimPS->SetAliveStatus(false);
	}

	ALastZonePlayerState* KillerPS = Killer ? Cast<ALastZonePlayerState>(Killer->PlayerState) : nullptr;
	if (KillerPS)
	{
		KillerPS->AddKill();
	}

	// Build and broadcast kill feed
	FKillFeedEntry FeedEntry;
	FeedEntry.VictimName = VictimPS ? VictimPS->GetPlayerName() : TEXT("Eliminated Player");
	FeedEntry.KillerName = KillerPS ? KillerPS->GetPlayerName() : TEXT("The Zone");
	FeedEntry.WeaponName = DamageCauser ? DamageCauser->GetName() : TEXT("Zone Collapse");
	FeedEntry.bIsHeadshot = false;

	if (ALastZoneGameState* ZoneGS = GetGameState<ALastZoneGameState>())
	{
		int32 Remaining = FMath::Max(0, ZoneGS->GetAlivePlayers() - 1);
		ZoneGS->SetAlivePlayersCount(Remaining);
		ZoneGS->MulticastBroadcastKillFeed(FeedEntry);

		if (VictimPS)
		{
			VictimPS->SetPlacement(Remaining + 1);
		}
	}

	// Show elimination results screen to victim controller
	if (ALastZonePlayerController* VictimPC = Cast<ALastZonePlayerController>(Victim))
	{
		FPlayerMatchResults Results;
		Results.Placement = VictimPS ? VictimPS->GetFinalPlacement() : 10;
		Results.TotalKills = VictimPS ? VictimPS->GetKills() : 0;
		Results.DamageDealt = VictimPS ? VictimPS->GetDamageDealt() : 0.0f;
		Results.bIsWinner = false;

		VictimPC->ClientShowMatchResults(Results);

		// Switch to spectate killer or teammates
		if (Killer && Killer->GetPawn())
		{
			VictimPC->ClientStartSpectating(Killer->GetPawn());
		}
	}

	CheckWinCondition();
}

void ALastZoneGameMode::CheckWinCondition()
{
	ALastZoneGameState* ZoneGS = GetGameState<ALastZoneGameState>();
	if (!ZoneGS) return;

	if (ZoneGS->GetZoneMatchState() != ELastZoneMatchState::ActiveBattle && ZoneGS->GetZoneMatchState() != ELastZoneMatchState::ZoneClosing)
	{
		return;
	}

	if (ZoneGS->GetAlivePlayers() <= 1)
	{
		// Find the last surviving controller
		AController* WinningController = nullptr;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (PC)
			{
				ALastZonePlayerState* PS = Cast<ALastZonePlayerState>(PC->PlayerState);
				if (PS && PS->IsPlayerAlive())
				{
					WinningController = PC;
					break;
				}
			}
		}

		EndMatch(WinningController);
	}
}

void ALastZoneGameMode::EndMatch(AController* Winner)
{
	if (ALastZoneGameState* ZoneGS = GetGameState<ALastZoneGameState>())
	{
		ZoneGS->SetZoneMatchState(ELastZoneMatchState::MatchEnded);
	}

	if (Winner)
	{
		ALastZonePlayerState* WinnerPS = Cast<ALastZonePlayerState>(Winner->PlayerState);
		if (WinnerPS)
		{
			WinnerPS->SetPlacement(1);
		}

		if (ALastZonePlayerController* WinnerPC = Cast<ALastZonePlayerController>(Winner))
		{
			FPlayerMatchResults WinnerResults;
			WinnerResults.Placement = 1;
			WinnerResults.TotalKills = WinnerPS ? WinnerPS->GetKills() : 0;
			WinnerResults.DamageDealt = WinnerPS ? WinnerPS->GetDamageDealt() : 0.0f;
			WinnerResults.bIsWinner = true; // "WINNER WINNER CHICKEN DINNER"

			WinnerPC->ClientShowMatchResults(WinnerResults);
		}
	}
}

void ALastZoneGameMode::RestartMatchSession()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

