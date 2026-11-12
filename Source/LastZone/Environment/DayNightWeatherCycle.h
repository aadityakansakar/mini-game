// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayNightWeatherCycle.generated.h"

class ADirectionalLight;
class ASkyAtmosphere;
class AVolumetricCloud;
class AExponentialHeightFog;

UENUM(BlueprintType)
enum class EWeatherCondition : uint8
{
	Clear        UMETA(DisplayName = "Clear Skies"),
	Overcast     UMETA(DisplayName = "Overcast"),
	Rainy        UMETA(DisplayName = "Heavy Rain"),
	DenseFog     UMETA(DisplayName = "Dense Fog")
};

UCLASS()
class LASTZONE_API ADayNightWeatherCycle : public AActor
{
	GENERATED_BODY()
	
public:	
	ADayNightWeatherCycle();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cycle")
	float TimeOfDayHours; // 0.0 to 24.0

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cycle")
	float DayLengthMinutes; // Real-world minutes for full 24h cycle

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weather")
	EWeatherCondition CurrentWeather;

	UPROPERTY(EditInstanceOnly, Category = "Environment Actors")
	ADirectionalLight* SunLightActor;

	UPROPERTY(EditInstanceOnly, Category = "Environment Actors")
	AExponentialHeightFog* FogActor;

	void UpdateSunPosition(float DeltaTime);
	void UpdateWeatherAtmosphere(float DeltaTime);

public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Weather")
	void SetWeatherCondition(EWeatherCondition NewWeather);
};

