// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneMatchEndWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void ULastZoneMatchEndWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &ULastZoneMatchEndWidget::OnRestartClicked);
	}
	if (LobbyButton)
	{
		LobbyButton->OnClicked.AddDynamic(this, &ULastZoneMatchEndWidget::OnLobbyClicked);
	}
}

void ULastZoneMatchEndWidget::SetMatchResults(const FPlayerMatchResults& Results)
{
	if (TitleText)
	{
		if (Results.bIsWinner)
		{
			TitleText->SetText(FText::FromString(TEXT("WINNER WINNER CHICKEN DINNER!")));
			TitleText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.84f, 0.0f, 1.0f))); // Gold
		}
		else
		{
			TitleText->SetText(FText::FromString(TEXT("BETTER LUCK NEXT TIME!")));
			TitleText->SetColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.2f, 0.2f, 1.0f))); // Red
		}
	}

	if (PlacementText)
	{
		PlacementText->SetText(FText::FromString(FString::Printf(TEXT("#%d / 50"), Results.Placement)));
	}
	if (KillsText)
	{
		KillsText->SetText(FText::AsNumber(Results.TotalKills));
	}
	if (DamageText)
	{
		DamageText->SetText(FText::AsNumber(FMath::RoundToInt(Results.DamageDealt)));
	}
	if (SurvivalTimeText)
	{
		int32 Mins = FMath::RoundToInt(Results.SurvivalTimeSeconds) / 60;
		int32 Secs = FMath::RoundToInt(Results.SurvivalTimeSeconds) % 60;
		SurvivalTimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Mins, Secs)));
	}
}

void ULastZoneMatchEndWidget::OnRestartClicked()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ULastZoneMatchEndWidget::OnLobbyClicked()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("Island_Main")), false);
}

