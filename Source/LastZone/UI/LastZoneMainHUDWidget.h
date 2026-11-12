// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LastZoneMainHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class ALastZoneCharacter;

UCLASS()
class LASTZONE_API ULastZoneMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* ArmorProgressBar;

	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* BoostProgressBar;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* WeaponNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* AlivePlayersText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* KillsText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ZoneTimerText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ZoneDistanceText;

	void UpdatePlayerStats();
	void UpdateWeaponInfo();
	void UpdateMatchStats();
	void UpdateSafeZoneInfo();
};

