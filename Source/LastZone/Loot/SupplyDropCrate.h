// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LastZone/LastZoneTypes.h"
#include "SupplyDropCrate.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UNiagaraComponent;
class USoundCue;

UCLASS()
class LASTZONE_API ASupplyDropCrate : public AActor
{
	GENERATED_BODY()
	
public:	
	ASupplyDropCrate();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CrateMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ParachuteMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* SmokeSignalFX;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* LandingThudSound;

	UPROPERTY(ReplicatedUsing = OnRep_HasLanded, BlueprintReadOnly, Category = "State")
	bool bHasLanded;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Loot")
	TArray<FLastZoneItem> CrateContents;

	UPROPERTY(EditDefaultsOnly, Category = "Descent")
	float DescentSpeed;

	UFUNCTION()
	void OnRep_HasLanded();

	UFUNCTION()
	void HandleLandingHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitializeSupplyDrop(const TArray<FLastZoneItem>& Contents);
};

