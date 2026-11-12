// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LastZone/LastZoneTypes.h"
#include "LastZonePlayerController.generated.h"

class UUserWidget;
class ALastZoneCharacter;

UCLASS()
class LASTZONE_API ALastZonePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALastZonePlayerController();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MatchEndWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	UPROPERTY()
	UUserWidget* MainHUDWidget;

	UPROPERTY()
	UUserWidget* InventoryWidget;

	UPROPERTY()
	UUserWidget* MatchEndWidget;

	UPROPERTY()
	UUserWidget* MobileControlsWidget;

	bool bIsInventoryOpen;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleInventoryScreen();

	UFUNCTION(Client, Reliable, Category = "UI")
	void ClientShowMatchResults(const FPlayerMatchResults& Results);

	UFUNCTION(Client, Reliable, Category = "Spectating")
	void ClientStartSpectating(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Spectating")
	void SpectateNextPlayer();

	UFUNCTION(BlueprintCallable, Category = "Platform")
	bool IsRunningOnMobile() const;
};

