// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LastZoneGrenadeProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;
class UNiagaraSystem;
class USoundCue;

UCLASS()
class LASTZONE_API ALastZoneGrenadeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ALastZoneGrenadeProjectile();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float FuseTime;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float MaxDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float MinDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float ImpulseStrength;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* ExplosionFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* ExplosionSound;

	FTimerHandle FuseTimerHandle;

	UFUNCTION()
	void Detonate();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastExplosionEffects();

public:
	void Throw(const FVector& ImpulseVector);
};

