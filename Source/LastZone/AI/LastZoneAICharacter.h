// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LastZone/Character/LastZoneCharacter.h"
#include "LastZoneAICharacter.generated.h"

UCLASS()
class LASTZONE_API ALastZoneAICharacter : public ALastZoneCharacter
{
	GENERATED_BODY()

public:
	ALastZoneAICharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
	float BotAimAccuracy; // 0.0 to 1.0

	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
	float BurstDuration;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
	float DelayBetweenBursts;

public:
	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	void BotFireAtTarget(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	void BotStopFiring();
};

