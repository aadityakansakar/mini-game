// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneMiniMapWidget.h"
#include "Rendering/DrawElements.h"
#include "LastZone/Core/LastZoneGameState.h"
#include "LastZone/Zone/SafeZoneManager.h"
#include "GameFramework/Pawn.h"

int32 ULastZoneMiniMapWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 CurrentLayer = LayerId;

	FVector2D WidgetCenter = AllottedGeometry.GetLocalSize() * 0.5f;
	float Radius = FMath::Min(WidgetCenter.X, WidgetCenter.Y) - 4.0f;

	// Background circle (Dark Tactical Green)
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		CurrentLayer++,
		AllottedGeometry.ToPaintGeometry(WidgetCenter - FVector2D(Radius, Radius), FVector2D(Radius * 2, Radius * 2)),
		FAppStyle::Get().GetBrush(TEXT("WhiteBrush")),
		ESlateDrawEffect::None,
		FLinearColor(0.04f, 0.08f, 0.05f, 0.85f)
	);

	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (!PlayerPawn) return CurrentLayer;

	FVector PawnWorld = PlayerPawn->GetActorLocation();
	FVector2D PlayerPosMeters(PawnWorld.X * 0.01f, PawnWorld.Y * 0.01f);

	UWorld* World = GetWorld();
	if (World)
	{
		ALastZoneGameState* GS = World->GetGameState<ALastZoneGameState>();
		if (GS && GS->GetSafeZoneManager())
		{
			ASafeZoneManager* Zone = GS->GetSafeZoneManager();

			// Draw White Circle for Target Safe Zone
			FVector2D TargetCenterMinimap = WorldToMiniMapCoords(Zone->GetTargetCenter(), PlayerPosMeters, Radius);
			float TargetRadiusPixels = (Zone->GetTargetRadius() / 1500.0f) * Radius;

			TArray<FVector2D> TargetPoints;
			const int32 CircleSegments = 32;
			for (int32 i = 0; i <= CircleSegments; ++i)
			{
				float Angle = (2.0f * PI / CircleSegments) * i;
				TargetPoints.Add(WidgetCenter + TargetCenterMinimap + FVector2D(FMath::Cos(Angle) * TargetRadiusPixels, FMath::Sin(Angle) * TargetRadiusPixels));
			}
			FSlateDrawElement::MakeLines(OutDrawElements, CurrentLayer++, AllottedGeometry.ToPaintGeometry(), TargetPoints, ESlateDrawEffect::None, FLinearColor::White, true, 2.0f);

			// Draw Blue Circle for Current Safe Zone
			FVector2D CurrentCenterMinimap = WorldToMiniMapCoords(Zone->GetCurrentCenter(), PlayerPosMeters, Radius);
			float CurrentRadiusPixels = (Zone->GetCurrentRadius() / 1500.0f) * Radius;

			TArray<FVector2D> CurrentPoints;
			for (int32 i = 0; i <= CircleSegments; ++i)
			{
				float Angle = (2.0f * PI / CircleSegments) * i;
				CurrentPoints.Add(WidgetCenter + CurrentCenterMinimap + FVector2D(FMath::Cos(Angle) * CurrentRadiusPixels, FMath::Sin(Angle) * CurrentRadiusPixels));
			}
			FSlateDrawElement::MakeLines(OutDrawElements, CurrentLayer++, AllottedGeometry.ToPaintGeometry(), CurrentPoints, ESlateDrawEffect::None, FLinearColor(0.2f, 0.6f, 1.0f, 1.0f), true, 2.5f);
		}
	}

	// Draw Player marker triangle at center
	float HeadingRad = FMath::DegreesToRadians(PlayerPawn->GetActorRotation().Yaw - 90.0f);
	FVector2D Tip = WidgetCenter + FVector2D(FMath::Cos(HeadingRad) * 8.0f, FMath::Sin(HeadingRad) * 8.0f);
	FVector2D Left = WidgetCenter + FVector2D(FMath::Cos(HeadingRad + 2.5f) * 6.0f, FMath::Sin(HeadingRad + 2.5f) * 6.0f);
	FVector2D Right = WidgetCenter + FVector2D(FMath::Cos(HeadingRad - 2.5f) * 6.0f, FMath::Sin(HeadingRad - 2.5f) * 6.0f);

	TArray<FVector2D> PlayerTriangle;
	PlayerTriangle.Add(Tip);
	PlayerTriangle.Add(Left);
	PlayerTriangle.Add(Right);
	PlayerTriangle.Add(Tip);

	FSlateDrawElement::MakeLines(OutDrawElements, CurrentLayer++, AllottedGeometry.ToPaintGeometry(), PlayerTriangle, ESlateDrawEffect::None, FLinearColor(1.0f, 0.85f, 0.1f, 1.0f), true, 2.0f);

	return CurrentLayer;
}

FVector2D ULastZoneMiniMapWidget::WorldToMiniMapCoords(const FVector2D& WorldPosMeters, const FVector2D& PlayerPosMeters, float MinimapRadiusPixels) const
{
	FVector2D Delta = WorldPosMeters - PlayerPosMeters;
	// Scale 1500 meters view radius to MinimapRadiusPixels
	float Scale = MinimapRadiusPixels / 1500.0f;
	return FVector2D(Delta.X * Scale, Delta.Y * Scale);
}

