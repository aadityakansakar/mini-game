// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "DayNightWeatherCycle.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/LightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Net/UnrealNetwork.h"

ADayNightWeatherCycle::ADayNightWeatherCycle()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	TimeOfDayHours = 8.5f; // Morning start 08:30
	DayLengthMinutes = 20.0f; // 20-minute match matches 24h cycle
	CurrentWeather = EWeatherCondition::Clear;
	SunLightActor = nullptr;
	FogActor = nullptr;
}

void ADayNightWeatherCycle::BeginPlay()
{
	Super::BeginPlay();
}

void ADayNightWeatherCycle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSunPosition(DeltaTime);
	UpdateWeatherAtmosphere(DeltaTime);
}

void ADayNightWeatherCycle::UpdateSunPosition(float DeltaTime)
{
	// Advance time of day
	float HoursPerSecond = 24.0f / (DayLengthMinutes * 60.0f);
	TimeOfDayHours = FMath::Fmod(TimeOfDayHours + (HoursPerSecond * DeltaTime), 24.0f);

	if (SunLightActor)
	{
		// Calculate sun pitch and yaw
		// 06:00 = sunrise (pitch 0), 12:00 = noon (pitch 90), 18:00 = sunset (pitch 180), 00:00 = midnight
		float SunPitch = ((TimeOfDayHours - 6.0f) / 12.0f) * 180.0f;
		float SunYaw = 45.0f;

		FRotator SunRotator(SunPitch, SunYaw, 0.0f);
		SunLightActor->SetActorRotation(SunRotator);

		// Adjust sun intensity when below horizon
		if (ULightComponent* LightComp = SunLightActor->GetLightComponent())
		{
			bool bIsDaytime = (TimeOfDayHours >= 5.5f && TimeOfDayHours <= 19.0f);
			float TargetIntensity = bIsDaytime ? 75000.0f : 150.0f; // Moonlight intensity at night
			LightComp->SetIntensity(TargetIntensity);
		}
	}
}

void ADayNightWeatherCycle::UpdateWeatherAtmosphere(float DeltaTime)
{
	if (!FogActor) return;

	UExponentialHeightFogComponent* FogComp = FogActor->GetComponent();
	if (!FogComp) return;

	float TargetFogDensity = 0.005f;
	switch (CurrentWeather)
	{
	case EWeatherCondition::Clear:    TargetFogDensity = 0.003f; break;
	case EWeatherCondition::Overcast: TargetFogDensity = 0.015f; break;
	case EWeatherCondition::Rainy:    TargetFogDensity = 0.035f; break;
	case EWeatherCondition::DenseFog: TargetFogDensity = 0.085f; break;
	}

	float CurrentDensity = FogComp->FogDensity;
	FogComp->SetFogDensity(FMath::FInterpTo(CurrentDensity, TargetFogDensity, DeltaTime, 0.2f));
}

void ADayNightWeatherCycle::SetWeatherCondition(EWeatherCondition NewWeather)
{
	CurrentWeather = NewWeather;
}

void ADayNightWeatherCycle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADayNightWeatherCycle, CurrentWeather);
}

