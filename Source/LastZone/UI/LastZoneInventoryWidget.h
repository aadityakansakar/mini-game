// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LastZone/LastZoneTypes.h"
#include "LastZoneInventoryWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UVerticalBox;
class UInventoryComponent;

UCLASS()
class LASTZONE_API ULastZoneInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* CapacityProgressBar;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* CapacityText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* Weapon1Text;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* Weapon2Text;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* SidearmText;

	UPROPERTY(meta = (BindWidgetOptional))
	UVerticalBox* ItemListBox;

	UFUNCTION()
	void RefreshInventoryDisplay();

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropSelectedItem(FName ItemID, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseSelectedConsumable(FName ItemID);
};

