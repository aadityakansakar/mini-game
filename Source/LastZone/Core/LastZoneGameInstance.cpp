// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneGameInstance.h"
#include "GameFramework/GameUserSettings.h"

ULastZoneGameInstance::ULastZoneGameInstance()
{
	MasterVolume = 1.0f;
	SFXVolume = 1.0f;
	OverallQualityLevel = 3; // Epic / Ultra for PC default
	DynamicResolutionTarget = 60.0f;
	PlayerName = TEXT("Survivor");
}

void ULastZoneGameInstance::Init()
{
	Super::Init();

	// Load user game settings
	if (UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		UserSettings->LoadSettings();
#if PLATFORM_ANDROID
		OverallQualityLevel = 1; // Medium for mobile
		UserSettings->SetOverallScalabilityLevel(1);
#else
		UserSettings->SetOverallScalabilityLevel(OverallQualityLevel);
#endif
		UserSettings->ApplySettings(false);
	}
}

void ULastZoneGameInstance::ApplyGraphicsSettings(int32 QualityLevel)
{
	OverallQualityLevel = FMath::Clamp(QualityLevel, 0, 3);
	if (UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		UserSettings->SetOverallScalabilityLevel(OverallQualityLevel);
		UserSettings->ApplySettings(true);
	}
}

void ULastZoneGameInstance::ApplyAudioSettings(float NewMasterVolume, float NewSFXVolume)
{
	MasterVolume = FMath::Clamp(NewMasterVolume, 0.0f, 1.0f);
	SFXVolume = FMath::Clamp(NewSFXVolume, 0.0f, 1.0f);
}

