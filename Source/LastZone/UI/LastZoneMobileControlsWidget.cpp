// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneMobileControlsWidget.h"
#include "Components/Button.h"
#include "LastZone/Character/LastZoneCharacter.h"

void ULastZoneMobileControlsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (FireButton)
	{
		FireButton->OnPressed.AddDynamic(this, &ULastZoneMobileControlsWidget::OnFirePressed);
		FireButton->OnReleased.AddDynamic(this, &ULastZoneMobileControlsWidget::OnFireReleased);
	}
	if (AimButton) AimButton->OnClicked.AddDynamic(this, &ULastZoneMobileControlsWidget::OnAimClicked);
	if (JumpButton) JumpButton->OnClicked.AddDynamic(this, &ULastZoneMobileControlsWidget::OnJumpClicked);
	if (CrouchButton) CrouchButton->OnClicked.AddDynamic(this, &ULastZoneMobileControlsWidget::OnCrouchClicked);
	if (ProneButton) ProneButton->OnClicked.AddDynamic(this, &ULastZoneMobileControlsWidget::OnProneClicked);
	if (SprintButton) SprintButton->OnClicked.AddDynamic(this, &ULastZoneMobileControlsWidget::OnSprintClicked);
	if (ReloadButton) ReloadButton->OnClicked.AddDynamic(this, &ULastZoneMobileControlsWidget::OnReloadClicked);
	if (Weapon1Button) Weapon1Button->OnClicked.AddDynamic(this, &ULastZoneMobileControlsWidget::OnWeapon1Clicked);
	if (Weapon2Button) Weapon2Button->OnClicked.AddDynamic(this, &ULastZoneMobileControlsWidget::OnWeapon2Clicked);
}

void ULastZoneMobileControlsWidget::OnFirePressed()
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		Char->StartFire();
	}
}

void ULastZoneMobileControlsWidget::OnFireReleased()
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		Char->StopFire();
	}
}

void ULastZoneMobileControlsWidget::OnAimClicked()
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		if (Char->IsAiming()) Char->StopAim();
		else Char->StartAim();
	}
}

void ULastZoneMobileControlsWidget::OnJumpClicked()
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		Char->Jump();
	}
}

void ULastZoneMobileControlsWidget::OnCrouchClicked()
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		Char->ToggleCrouch();
	}
}

void ULastZoneMobileControlsWidget::OnProneClicked()
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		Char->ToggleProne();
	}
}

void ULastZoneMobileControlsWidget::OnSprintClicked()
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		Char->StartSprint();
	}
}

void ULastZoneMobileControlsWidget::OnReloadClicked()
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		Char->Reload();
	}
}

void ULastZoneMobileControlsWidget::OnWeapon1Clicked()
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		Char->SelectPrimaryWeapon();
	}
}

void ULastZoneMobileControlsWidget::OnWeapon2Clicked()
{
	if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(GetOwningPlayerPawn()))
	{
		Char->SelectSecondaryWeapon();
	}
}

