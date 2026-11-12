// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LastZone/LastZoneTypes.h"
#include "LastZonePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillsChangedSignature, int32, NewKills);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAliveStatusChangedSignature, bool, bIsAlive);

UCLASS()
class LASTZONE_API ALastZonePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ALastZonePlayerState();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Kills, BlueprintReadOnly, Category = "Stats")
	int32 KillCount;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	float DamageDealt;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	int32 HeadshotCount;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	int32 TeamID;

	UPROPERTY(ReplicatedUsing = OnRep_AliveStatus, BlueprintReadOnly, Category = "Stats")
	bool bIsAlive;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	int32 FinalPlacement;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	float SurvivalTimeSeconds;

	UFUNCTION()
	void OnRep_Kills();

	UFUNCTION()
	void OnRep_AliveStatus();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Stats")
	void AddKill();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Stats")
	void AddDamage(float Damage);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Stats")
	void SetAliveStatus(bool bAlive);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Stats")
	void SetPlacement(int32 Placement);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Stats")
	void SetTeamID(int32 InTeamID) { TeamID = InTeamID; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetKills() const { return KillCount; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetDamageDealt() const { return DamageDealt; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetTeamID() const { return TeamID; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool IsPlayerAlive() const { return bIsAlive; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetFinalPlacement() const { return FinalPlacement; }

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnKillsChangedSignature OnKillsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAliveStatusChangedSignature OnAliveStatusChanged;
};

