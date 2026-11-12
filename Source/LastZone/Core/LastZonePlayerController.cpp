// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZonePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "LastZone/Character/LastZoneCharacter.h"
#include "LastZone/Core/LastZonePlayerState.h"
#include "LastZone/UI/LastZoneHUD.h"

ALastZonePlayerController::ALastZonePlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableTouchEvents = true;
	bIsInventoryOpen = false;
}

void ALastZonePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		// Spawn in-game HUD
		if (MainHUDWidgetClass)
		{
			MainHUDWidget = CreateWidget<UUserWidget>(this, MainHUDWidgetClass);
			if (MainHUDWidget)
			{
				MainHUDWidget->AddToViewport(0);
			}
		}

		// Spawn mobile controls if on mobile platform
		if (IsRunningOnMobile() && MobileControlsWidgetClass)
		{
			MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);
			if (MobileControlsWidget)
			{
				MobileControlsWidget->AddToViewport(10);
			}
		}
	}
}

void ALastZonePlayerController::ToggleInventoryScreen()
{
	bIsInventoryOpen = !bIsInventoryOpen;

	if (bIsInventoryOpen)
	{
		if (!InventoryWidget && InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
		}

		if (InventoryWidget)
		{
			InventoryWidget->AddToViewport(5);
			SetInputMode(FInputModeGameAndUI());
			bShowMouseCursor = true;
		}
	}
	else
	{
		if (InventoryWidget)
		{
			InventoryWidget->RemoveFromParent();
		}
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
}

void ALastZonePlayerController::ClientShowMatchResults_Implementation(const FPlayerMatchResults& Results)
{
	if (!MatchEndWidget && MatchEndWidgetClass)
	{
		MatchEndWidget = CreateWidget<UUserWidget>(this, MatchEndWidgetClass);
	}

	if (MatchEndWidget)
	{
		MatchEndWidget->AddToViewport(20);
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}

void ALastZonePlayerController::ClientStartSpectating_Implementation(AActor* TargetActor)
{
	UnPossess();
	SetViewTargetWithBlend(TargetActor, 1.0f);
}

void ALastZonePlayerController::SpectateNextPlayer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (TActorIterator<ALastZoneCharacter> It(World); It; ++It)
	{
		ALastZoneCharacter* OtherChar = *It;
		if (OtherChar && OtherChar != GetPawn() && OtherChar->GetPlayerState() && Cast<ALastZonePlayerState>(OtherChar->GetPlayerState())->IsPlayerAlive())
		{
			SetViewTargetWithBlend(OtherChar, 0.5f);
			break;
		}
	}
}

bool ALastZonePlayerController::IsRunningOnMobile() const
{
#if PLATFORM_ANDROID || PLATFORM_IOS
	return true;
#else
	return false;
#endif
}

