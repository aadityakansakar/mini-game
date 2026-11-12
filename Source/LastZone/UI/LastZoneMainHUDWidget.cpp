// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneMainHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "LastZone/Character/LastZoneCharacter.h"
#include "LastZone/Character/Components/HealthComponent.h"
#include "LastZone/Character/Components/ArmorComponent.h"
#include "LastZone/Character/Components/StaminaComponent.h"
#include "LastZone/Character/Components/InventoryComponent.h"
#include "LastZone/Weapons/LastZoneWeaponBase.h"
#include "LastZone/Core/LastZoneGameState.h"
#include "LastZone/Core/LastZonePlayerState.h"
#include "LastZone/Zone/SafeZoneManager.h"

void ULastZoneMainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULastZoneMainHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePlayerStats();
	UpdateWeaponInfo();
	UpdateMatchStats();
	UpdateSafeZoneInfo();
}

void ULastZoneMainHUDWidget::UpdatePlayerStats()
{
	ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn());
	if (!Char) return;

	if (Char->GetHealthComponent())
	{
		float HealthPercent = Char->GetHealthComponent()->GetHealthPercent();
		if (HealthProgressBar) HealthProgressBar->SetPercent(HealthPercent);
		if (HealthText) HealthText->SetText(FText::AsNumber(FMath::RoundToInt(Char->GetHealthComponent()->GetHealth())));

		if (BoostProgressBar) BoostProgressBar->SetPercent(Char->GetHealthComponent()->GetBoost() / 100.0f);
	}

	if (Char->GetArmorComponent())
	{
		float ArmorPercent = Char->GetArmorComponent()->GetVestPercent();
		if (ArmorProgressBar) ArmorProgressBar->SetPercent(ArmorPercent);
	}

	if (Char->GetStaminaComponent())
	{
		float StaminaPercent = Char->GetStaminaComponent()->GetStaminaPercent();
		if (StaminaProgressBar) StaminaProgressBar->SetPercent(StaminaPercent);
	}
}

void ULastZoneMainHUDWidget::UpdateWeaponInfo()
{
	ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn());
	if (!Char || !Char->GetInventoryComponent()) return;

	ALastZoneWeaponBase* CurrentWeapon = Char->GetInventoryComponent()->GetCurrentWeapon();
	if (CurrentWeapon)
	{
		if (WeaponNameText) WeaponNameText->SetText(CurrentWeapon->GetStats().WeaponName);
		if (AmmoText)
		{
			FString AmmoStr = FString::Printf(TEXT("%d / %d"), CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetReserveAmmo());
			AmmoText->SetText(FText::FromString(AmmoStr));
		}
	}
	else
	{
		if (WeaponNameText) WeaponNameText->SetText(FText::FromString(TEXT("Unarmed")));
		if (AmmoText) AmmoText->SetText(FText::FromString(TEXT("- / -")));
	}
}

void ULastZoneMainHUDWidget::UpdateMatchStats()
{
	if (UWorld* World = GetWorld())
	{
		if (ALastZoneGameState* GS = World->GetGameState<ALastZoneGameState>())
		{
			if (AlivePlayersText) AlivePlayersText->SetText(FText::AsNumber(GS->GetAlivePlayers()));
		}
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ALastZonePlayerState* PS = Cast<ALastZonePlayerState>(PC->PlayerState))
		{
			if (KillsText) KillsText->SetText(FText::AsNumber(PS->GetKills()));
		}
	}
}

void ULastZoneMainHUDWidget::UpdateSafeZoneInfo()
{
	UWorld* World = GetWorld();
	if (!World) return;

	ALastZoneGameState* GS = World->GetGameState<ALastZoneGameState>();
	if (!GS || !GS->GetSafeZoneManager()) return;

	ASafeZoneManager* Zone = GS->GetSafeZoneManager();

	if (ZoneTimerText)
	{
		int32 SecondsRemaining = FMath::Max(0, FMath::RoundToInt(Zone->GetTimeRemainingInPhase()));
		int32 Minutes = SecondsRemaining / 60;
		int32 Seconds = SecondsRemaining % 60;
		FString TimerStr = FString::Printf(TEXT("%02d:%02d %s"), Minutes, Seconds, Zone->IsShrinking() ? TEXT("[SHRINKING]") : TEXT("[SAFE]"));
		ZoneTimerText->SetText(FText::FromString(TimerStr));
	}

	if (ZoneDistanceText && GetOwningPlayerPawn())
	{
		float DistanceMeters = Zone->GetDistanceToSafeZoneEdge(GetOwningPlayerPawn()->GetActorLocation());
		if (DistanceMeters < 0.0f)
		{
			ZoneDistanceText->SetText(FText::FromString(FString::Printf(TEXT("OUTSIDE ZONE: %.0f m"), -DistanceMeters)));
		}
		else
		{
			ZoneDistanceText->SetText(FText::FromString(FString::Printf(TEXT("ZONE EDGE: %.0f m"), DistanceMeters)));
		}
	}
}

