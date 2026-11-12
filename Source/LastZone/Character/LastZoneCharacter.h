// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "LastZone/LastZoneTypes.h"
#include "LastZoneCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UHealthComponent;
class UArmorComponent;
class UStaminaComponent;
class UInventoryComponent;
class UInputMappingContext;
class UInputAction;
class ALastZoneVehicleBase;

UCLASS()
class LASTZONE_API ALastZoneCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALastZoneCharacter();

protected:
	virtual void BeginPlay() override;

	// Camera components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	// Gameplay Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArmorComponent* ArmorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaminaComponent* StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

	// Input Actions (Enhanced Input)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ProneAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* AimAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ReloadAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SwitchWeapon1Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SwitchWeapon2Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SwitchSidearmAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleInventoryAction;

	// Movement Speeds
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float JogSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float CrouchSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ProneSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SwimSpeed;

	// Movement & Stance State
	UPROPERTY(ReplicatedUsing = OnRep_PlayerStance, BlueprintReadOnly, Category = "Movement")
	EPlayerStance PlayerStance;

	UPROPERTY(ReplicatedUsing = OnRep_MovementState, BlueprintReadOnly, Category = "Movement")
	EPlayerMovementState PlayerMovementState;

	UPROPERTY(ReplicatedUsing = OnRep_IsAiming, BlueprintReadOnly, Category = "Combat")
	bool bIsAiming;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	float AimPitch;

	// Vehicle seating
	UPROPERTY(ReplicatedUsing = OnRep_CurrentVehicle, BlueprintReadOnly, Category = "Vehicle")
	ALastZoneVehicleBase* CurrentVehicle;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Vehicle")
	int32 CurrentSeatIndex;

	// Vaulting parameters
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Vaulting")
	float VaultTraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Vaulting")
	float MaxVaultHeight;

	bool bIsVaulting;
	FVector VaultStartPos;
	FVector VaultEndPos;
	float VaultTimelineProgress;

	// Recoil recovery
	float TargetRecoilPitch;
	float TargetRecoilYaw;
	float AccumulatedRecoilPitch;
	float AccumulatedRecoilYaw;

	// Camera ADS targets
	FVector DefaultCameraSocketOffset;
	FVector ADSCameraSocketOffset;
	float DefaultFOV;
	float CurrentADSAlpha;

	UFUNCTION()
	void OnRep_PlayerStance();

	UFUNCTION()
	void OnRep_MovementState();

	UFUNCTION()
	void OnRep_IsAiming();

	UFUNCTION()
	void OnRep_CurrentVehicle();

	UFUNCTION()
	void HandleCharacterDeath(AActor* Killer, AActor* Victim);

	UFUNCTION()
	void HandleCharacterDowned(AActor* DownedActor);

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Input Handlers
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartSprint();
	void StopSprint();
	void ToggleCrouch();
	void ToggleProne();
	void StartFire();
	void StopFire();
	void StartAim();
	void StopAim();
	void Reload();
	void Interact();
	void SelectPrimaryWeapon();
	void SelectSecondaryWeapon();
	void SelectSidearm();
	void ToggleInventory();

	// Server RPCs for Stance & Combat
	UFUNCTION(Server, Reliable)
	void ServerSetStance(EPlayerStance NewStance);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bNewAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAimPitch(float NewPitch);

	UFUNCTION(Server, Reliable)
	void ServerTryVault();

	UFUNCTION(Server, Reliable)
	void ServerInteractWithObject();

	// Vaulting execution
	bool CheckVaultObstacle(FVector& OutVaultLanding);
	void ExecuteVault(const FVector& LandingPos);

	// Vehicle Interaction
	void EnterVehicle(ALastZoneVehicleBase* Vehicle, int32 SeatIndex);
	void ExitVehicle();

	// Recoil manipulation
	void AddRecoilInput(float PitchRecoil, float YawRecoil);

	// Accessors
	UFUNCTION(BlueprintCallable, Category = "Character")
	EPlayerStance GetPlayerStance() const { return PlayerStance; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	EPlayerMovementState GetPlayerMovementState() const { return PlayerMovementState; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool IsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	float GetAimPitch() const { return AimPitch; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	ALastZoneVehicleBase* GetCurrentVehicle() const { return CurrentVehicle; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	UArmorComponent* GetArmorComponent() const { return ArmorComponent; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
};

