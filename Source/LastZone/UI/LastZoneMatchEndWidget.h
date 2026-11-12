// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LastZone/LastZoneTypes.h"
#include "LastZoneMatchEndWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class LASTZONE_API ULastZoneMatchEndWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "MatchResults")
	void SetMatchResults(const FPlayerMatchResults& Results);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* PlacementText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* KillsText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* DamageText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* SurvivalTimeText;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* RestartButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* LobbyButton;

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnLobbyClicked();
};

