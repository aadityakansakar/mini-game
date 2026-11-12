// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LastZoneHUD.generated.h"

UCLASS()
class LASTZONE_API ALastZoneHUD : public AHUD
{
	GENERATED_BODY()

public:
	ALastZoneHUD();

	virtual void DrawHUD() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	float CrosshairSpread;

	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	FLinearColor CrosshairColor;

	void DrawDynamicCrosshair();
	void DrawCompassStrip();
};

