// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LastZone/LastZoneTypes.h"
#include "LastZoneWeaponBase.generated.h"

class USkeletalMeshComponent;
class UNiagaraSystem;
class USoundCue;
class ALastZoneCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, int32, CurrentAmmo, int32, ReserveAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadStateSignature, bool, bIsReloading);

UCLASS(Abstract)
class LASTZONE_API ALastZoneWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ALastZoneWeaponBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	FWeaponStats WeaponStats;

	UPROPERTY(ReplicatedUsing = OnRep_Ammo, BlueprintReadOnly, Category = "Ammo")
	int32 CurrentAmmoInClip;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ammo")
	int32 ReserveAmmo;

	UPROPERTY(ReplicatedUsing = OnRep_IsReloading, BlueprintReadOnly, Category = "State")
	bool bIsReloading;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsFiring;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* MuzzleFlashFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* EmptyClickSound;

	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;

	UFUNCTION()
	void OnRep_Ammo();

	UFUNCTION()
	void OnRep_IsReloading();

	// Procedural Recoil offsets
	float CurrentRecoilPitch;
	float CurrentRecoilYaw;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Core actions
	virtual void StartFire();
	virtual void StopFire();
	virtual void FireShot();

	virtual void StartReload();
	virtual void FinishReload();
	virtual void CancelReload();

	virtual bool CanFire() const;
	virtual bool CanReload() const;

	// Server RPCs
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire();

	UFUNCTION(Server, Reliable)
	void ServerStopFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReload();

	// Multicast effects
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayFireEffects();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayReloadEffects();

	// Overridable firing implementation (spawning projectile or line tracing)
	virtual void ExecuteFireLogic(const FVector& MuzzleLocation, const FRotator& FireRotation);

	// Calculate fire spread based on player state
	FVector CalculateSpreadVector(const FVector& BaseForward, bool bIsAiming, EPlayerStance Stance, bool bIsMoving) const;

	// Accessors
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	const FWeaponStats& GetStats() const { return WeaponStats; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmoInClip; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetReserveAmmo() const { return ReserveAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsReloading() const { return bIsReloading; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	FVector GetMuzzleLocation() const;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmmoChangedSignature OnAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloadStateSignature OnReloadStateChanged;
};

