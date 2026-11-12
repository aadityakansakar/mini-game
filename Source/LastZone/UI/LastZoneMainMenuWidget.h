// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LastZoneMainMenuWidget.generated.h"

class UButton;

UCLASS()
class LASTZONE_API ULastZoneMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* PlayButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* PracticeButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* SettingsButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ExitButton;

	UFUNCTION()
	void OnPlayClicked();

	UFUNCTION()
	void OnPracticeClicked();

	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnExitClicked();
};

