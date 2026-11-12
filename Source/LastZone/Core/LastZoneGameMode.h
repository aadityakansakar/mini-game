// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LastZone/LastZoneTypes.h"
#include "LastZoneGameMode.generated.h"

class ASafeZoneManager;
class ALootManager;
class ALastZoneAICharacter;
class ALastZoneCharacter;

UCLASS()
class LASTZONE_API ALastZoneGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALastZoneGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	UPROPERTY(EditDefaultsOnly, Category = "BattleRoyale|Match")
	int32 TargetPlayerCount;

	UPROPERTY(EditDefaultsOnly, Category = "BattleRoyale|Match")
	float WarmupDuration;

	UPROPERTY(EditDefaultsOnly, Category = "BattleRoyale|Match")
	bool bEnableBots;

	UPROPERTY(EditDefaultsOnly, Category = "BattleRoyale|Match")
	int32 BotCount;

	UPROPERTY(EditDefaultsOnly, Category = "BattleRoyale|Classes")
	TSubclassOf<ASafeZoneManager> SafeZoneManagerClass;

	UPROPERTY(EditDefaultsOnly, Category = "BattleRoyale|Classes")
	TSubclassOf<ALootManager> LootManagerClass;

	UPROPERTY(EditDefaultsOnly, Category = "BattleRoyale|Classes")
	TSubclassOf<ALastZoneAICharacter> BotCharacterClass;

	UPROPERTY()
	ASafeZoneManager* ActiveSafeZoneManager;

	UPROPERTY()
	ALootManager* ActiveLootManager;

	FTimerHandle WarmupTimerHandle;
	float CurrentWarmupTimeRemaining;

	void StartWarmup();
	void WarmupCountdownTick();
	void StartMatch();
	void SpawnBots();

public:
	virtual void Tick(float DeltaTime) override;

	// Called when a character is eliminated
	UFUNCTION(BlueprintCallable, Category = "BattleRoyale")
	void OnCharacterEliminated(AController* Killer, AController* Victim, AActor* DamageCauser);

	// Checks if only 1 player / team remains
	void CheckWinCondition();

	void EndMatch(AController* Winner);

	UFUNCTION(BlueprintCallable, Category = "BattleRoyale")
	void RestartMatchSession();
};

