// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneInventoryWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "LastZone/Character/LastZoneCharacter.h"
#include "LastZone/Character/Components/InventoryComponent.h"
#include "LastZone/Weapons/LastZoneWeaponBase.h"

void ULastZoneInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshInventoryDisplay();

	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		if (UInventoryComponent* Inv = Char->GetInventoryComponent())
		{
			Inv->OnInventoryUpdated.AddDynamic(this, &ULastZoneInventoryWidget::RefreshInventoryDisplay);
		}
	}
}

void ULastZoneInventoryWidget::RefreshInventoryDisplay()
{
	ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn());
	if (!Char || !Char->GetInventoryComponent()) return;

	UInventoryComponent* Inv = Char->GetInventoryComponent();

	// Capacity bar
	float Weight = Inv->GetCurrentWeight();
	float MaxCap = Inv->GetMaxCapacity();
	if (CapacityProgressBar) CapacityProgressBar->SetPercent(MaxCap > 0.0f ? Weight / MaxCap : 0.0f);
	if (CapacityText) CapacityText->SetText(FText::FromString(FString::Printf(TEXT("%.1f / %.1f kg"), Weight, MaxCap)));

	// Weapon slots
	if (Weapon1Text)
	{
		ALastZoneWeaponBase* W1 = Inv->GetWeaponInSlot(0);
		Weapon1Text->SetText(W1 ? W1->GetStats().WeaponName : FText::FromString(TEXT("Empty Slot 1")));
	}
	if (Weapon2Text)
	{
		ALastZoneWeaponBase* W2 = Inv->GetWeaponInSlot(1);
		Weapon2Text->SetText(W2 ? W2->GetStats().WeaponName : FText::FromString(TEXT("Empty Slot 2")));
	}
	if (SidearmText)
	{
		ALastZoneWeaponBase* W3 = Inv->GetWeaponInSlot(2);
		SidearmText->SetText(W3 ? W3->GetStats().WeaponName : FText::FromString(TEXT("Empty Sidearm")));
	}
}

void ULastZoneInventoryWidget::DropSelectedItem(FName ItemID, int32 Quantity)
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		if (UInventoryComponent* Inv = Char->GetInventoryComponent())
		{
			Inv->ServerDropItem(ItemID, Quantity);
		}
	}
}

void ULastZoneInventoryWidget::UseSelectedConsumable(FName ItemID)
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		if (UInventoryComponent* Inv = Char->GetInventoryComponent())
		{
			Inv->ServerUseConsumable(ItemID);
		}
	}
}

