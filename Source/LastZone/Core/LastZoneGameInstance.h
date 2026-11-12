// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LastZoneGameInstance.generated.h"

UCLASS()
class LASTZONE_API ULastZoneGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	ULastZoneGameInstance();

protected:
	virtual void Init() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Audio")
	float MasterVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Audio")
	float SFXVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Graphics")
	int32 OverallQualityLevel; // 0 = Low, 1 = Medium, 2 = High, 3 = Epic / Ultra

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Graphics")
	float DynamicResolutionTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Player")
	FString PlayerName;

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplyGraphicsSettings(int32 QualityLevel);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplyAudioSettings(float NewMasterVolume, float NewSFXVolume);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetPlayerProfileName(const FString& NewName) { PlayerName = NewName; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FString GetPlayerProfileName() const { return PlayerName; }
};

