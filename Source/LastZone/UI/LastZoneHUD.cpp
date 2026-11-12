// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneHUD.h"
#include "Engine/Canvas.h"
#include "LastZone/Character/LastZoneCharacter.h"
#include "LastZone/Weapons/LastZoneWeaponBase.h"
#include "LastZone/Character/Components/InventoryComponent.h"

ALastZoneHUD::ALastZoneHUD()
{
	CrosshairSpread = 12.0f;
	CrosshairColor = FLinearColor(0.95f, 0.85f, 0.2f, 0.9f); // Military Gold
}

void ALastZoneHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas) return;

	DrawDynamicCrosshair();
	DrawCompassStrip();
}

void ALastZoneHUD::DrawDynamicCrosshair()
{
	ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPawn());
	if (!Char || Char->IsAiming()) return; // Hide crosshair in ADS

	float CenterX = Canvas->ClipX * 0.5f;
	float CenterY = Canvas->ClipY * 0.5f;

	float Spread = CrosshairSpread;
	if (Char->GetVelocity().Size() > 10.0f)
	{
		Spread *= 1.8f;
	}
	if (Char->GetPlayerStance() == EPlayerStance::Crouching)
	{
		Spread *= 0.7f;
	}

	float LineLength = 10.0f;
	float LineThickness = 2.0f;

	// Top line
	DrawRect(CrosshairColor, CenterX - (LineThickness * 0.5f), CenterY - Spread - LineLength, LineThickness, LineLength);
	// Bottom line
	DrawRect(CrosshairColor, CenterX - (LineThickness * 0.5f), CenterY + Spread, LineThickness, LineLength);
	// Left line
	DrawRect(CrosshairColor, CenterX - Spread - LineLength, CenterY - (LineThickness * 0.5f), LineLength, LineThickness);
	// Right line
	DrawRect(CrosshairColor, CenterX + Spread, CenterY - (LineThickness * 0.5f), LineLength, LineThickness);
	// Center dot
	DrawRect(CrosshairColor, CenterX - 1.5f, CenterY - 1.5f, 3.0f, 3.0f);
}

void ALastZoneHUD::DrawCompassStrip()
{
	if (!GetOwningPlayerController()) return;

	float CenterX = Canvas->ClipX * 0.5f;
	float CompassY = 30.0f;
	float CompassWidth = 360.0f;

	// Background strip (Dark military green)
	DrawRect(FLinearColor(0.04f, 0.08f, 0.05f, 0.75f), CenterX - (CompassWidth * 0.5f), CompassY, CompassWidth, 24.0f);

	// Cardinal center tick (Gold)
	DrawRect(FLinearColor(0.85f, 0.75f, 0.15f, 1.0f), CenterX - 1.0f, CompassY, 2.0f, 24.0f);

	// Display heading degrees
	float Heading = GetOwningPlayerController()->GetControlRotation().Yaw;
	if (Heading < 0.0f) Heading += 360.0f;

	FString HeadingStr = FString::Printf(TEXT("%03d"), FMath::RoundToInt(Heading));
	DrawText(HeadingStr, FLinearColor::White, CenterX - 12.0f, CompassY + 28.0f);
}

