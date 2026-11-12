// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LastZoneMiniMapWidget.generated.h"

class UImage;

UCLASS()
class LASTZONE_API ULastZoneMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "MiniMap")
	float MapRadiusMeters;

	UPROPERTY(EditDefaultsOnly, Category = "MiniMap")
	FLinearColor CurrentZoneColor;

	UPROPERTY(EditDefaultsOnly, Category = "MiniMap")
	FLinearColor NextZoneColor;

	UPROPERTY(EditDefaultsOnly, Category = "MiniMap")
	FLinearColor PlayerMarkerColor;

	FVector2D WorldToMiniMapCoords(const FVector2D& WorldPosMeters, const FVector2D& PlayerPosMeters, float MinimapRadiusPixels) const;
};

