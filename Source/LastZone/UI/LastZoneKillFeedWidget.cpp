// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneKillFeedWidget.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "LastZone/Core/LastZoneGameState.h"

void ULastZoneKillFeedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		if (ALastZoneGameState* GS = World->GetGameState<ALastZoneGameState>())
		{
			GS->OnKillFeedBroadcast.AddDynamic(this, &ULastZoneKillFeedWidget::AddKillFeedEntry);
		}
	}
}

void ULastZoneKillFeedWidget::AddKillFeedEntry(const FKillFeedEntry& Entry)
{
	if (!KillFeedContainer) return;

	UTextBlock* EntryText = NewObject<UTextBlock>(this);
	if (EntryText)
	{
		FString Message = FString::Printf(TEXT("%s [%s] %s"), *Entry.KillerName, *Entry.WeaponName, *Entry.VictimName);
		EntryText->SetText(FText::FromString(Message));
		EntryText->SetColorAndOpacity(FSlateColor(Entry.bIsHeadshot ? FLinearColor(1.0f, 0.2f, 0.2f, 1.0f) : FLinearColor::White));

		KillFeedContainer->AddChildToVerticalBox(EntryText);

		// Maintain max 5 items in killfeed
		if (KillFeedContainer->GetChildrenCount() > 5)
		{
			KillFeedContainer->RemoveChildAt(0);
		}
	}
}

