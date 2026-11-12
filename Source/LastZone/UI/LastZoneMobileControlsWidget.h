// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LastZoneMobileControlsWidget.generated.h"

class UButton;

UCLASS()
class LASTZONE_API ULastZoneMobileControlsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* FireButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* AimButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* JumpButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* CrouchButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ProneButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* SprintButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ReloadButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* Weapon1Button;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* Weapon2Button;

	// Button Handlers
	UFUNCTION()
	void OnFirePressed();

	UFUNCTION()
	void OnFireReleased();

	UFUNCTION()
	void OnAimClicked();

	UFUNCTION()
	void OnJumpClicked();

	UFUNCTION()
	void OnCrouchClicked();

	UFUNCTION()
	void OnProneClicked();

	UFUNCTION()
	void OnSprintClicked();

	UFUNCTION()
	void OnReloadClicked();

	UFUNCTION()
	void OnWeapon1Clicked();

	UFUNCTION()
	void OnWeapon2Clicked();
};

