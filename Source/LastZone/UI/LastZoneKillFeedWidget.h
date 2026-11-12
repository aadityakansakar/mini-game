// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LastZone/LastZoneTypes.h"
#include "LastZoneKillFeedWidget.generated.h"

class UVerticalBox;

UCLASS()
class LASTZONE_API ULastZoneKillFeedWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UVerticalBox* KillFeedContainer;

	UFUNCTION()
	void AddKillFeedEntry(const FKillFeedEntry& Entry);
};

