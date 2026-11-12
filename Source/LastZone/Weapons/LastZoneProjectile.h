// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LastZone/LastZoneTypes.h"
#include "LastZoneProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class USoundCue;

UCLASS()
class LASTZONE_API ALastZoneProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ALastZoneProjectile();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* ImpactFleshFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* ImpactDefaultFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* ImpactFleshSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* ImpactDefaultSound;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Damage")
	float Damage;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Damage")
	float HeadshotMultiplier;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	void InitializeProjectile(float InDamage, float InHeadshotMultiplier, float InitialSpeed, float GravityScale, TSubclassOf<UDamageType> InDamageType);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

