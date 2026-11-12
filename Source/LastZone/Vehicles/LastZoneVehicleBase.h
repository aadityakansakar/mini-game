// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LastZoneVehicleBase.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UNiagaraSystem;
class USoundCue;
class ALastZoneCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVehicleHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVehicleDestroyedSignature, ALastZoneVehicleBase*, Vehicle);

UCLASS()
class LASTZONE_API ALastZoneVehicleBase : public APawn
{
	GENERATED_BODY()

public:
	ALastZoneVehicleBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* VehicleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* FollowCamera;

	// Vehicle Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Stats")
	float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Vehicle|Stats")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Stats")
	float MaxFuel;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Vehicle|Stats")
	float CurrentFuel;

	UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Stats")
	float FuelConsumptionRate; // Liters per second at full throttle

	UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Stats")
	int32 MaxSeats;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Vehicle|Seating")
	TArray<ALastZoneCharacter*> SeatedPassengers;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Vehicle|Input")
	float ThrottleInput;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Vehicle|Input")
	float SteeringInput;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Vehicle|Input")
	float BrakeInput;

	UPROPERTY(ReplicatedUsing = OnRep_IsDestroyed, BlueprintReadOnly, Category = "Vehicle|State")
	bool bIsDestroyed;

	// Effects
	UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Effects")
	UNiagaraSystem* ExplosionFX;

	UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Effects")
	USoundCue* EngineSound;

	UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Effects")
	USoundCue* ExplosionSound;

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_IsDestroyed();

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Input controls
	UFUNCTION(BlueprintCallable, Category = "Vehicle|Input")
	void SetThrottle(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle|Input")
	void SetSteering(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle|Input")
	void SetBrake(float Value);

	UFUNCTION(Server, Reliable)
	void ServerUpdateVehicleInput(float InThrottle, float InSteering, float InBrake);

	// Seating
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Vehicle|Seating")
	bool RequestEnter(ALastZoneCharacter* Passenger, int32 PreferredSeat = 0);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Vehicle|Seating")
	void RequestExit(ALastZoneCharacter* Passenger);

	UFUNCTION(Server, Reliable)
	void ServerExitVehicle(ALastZoneCharacter* Passenger);

	// Physics updates
	virtual void UpdateVehiclePhysics(float DeltaTime);

	// Accessors
	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	float GetHealthPercent() const { return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	float GetFuelPercent() const { return MaxFuel > 0.0f ? CurrentFuel / MaxFuel : 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	float GetForwardSpeedKmH() const;

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	bool IsDestroyed() const { return bIsDestroyed; }

	UPROPERTY(BlueprintAssignable, Category = "Vehicle|Events")
	FOnVehicleHealthChangedSignature OnVehicleHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Vehicle|Events")
	FOnVehicleDestroyedSignature OnVehicleDestroyed;
};

