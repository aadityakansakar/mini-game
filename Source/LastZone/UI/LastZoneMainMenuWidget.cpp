// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void ULastZoneMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PlayButton) PlayButton->OnClicked.AddDynamic(this, &ULastZoneMainMenuWidget::OnPlayClicked);
	if (PracticeButton) PracticeButton->OnClicked.AddDynamic(this, &ULastZoneMainMenuWidget::OnPracticeClicked);
	if (SettingsButton) SettingsButton->OnClicked.AddDynamic(this, &ULastZoneMainMenuWidget::OnSettingsClicked);
	if (ExitButton) ExitButton->OnClicked.AddDynamic(this, &ULastZoneMainMenuWidget::OnExitClicked);
}

void ULastZoneMainMenuWidget::OnPlayClicked()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("Island_Main")), false);
}

void ULastZoneMainMenuWidget::OnPracticeClicked()
{
	// Practice mode with bot backfill
	UGameplayStatics::OpenLevel(this, FName(TEXT("Island_Main")), false, TEXT("?bEnableBots=1?BotCount=45"));
}

void ULastZoneMainMenuWidget::OnSettingsClicked()
{
	// Open settings dialog
}

void ULastZoneMainMenuWidget::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

