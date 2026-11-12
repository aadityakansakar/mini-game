// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChangedSignature, float, CurrentStamina, float, MaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaExhaustedSignature, bool, bIsExhausted);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LASTZONE_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStaminaComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Stamina, EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float SprintDrainRate; // Drain per second while sprinting

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float RecoveryRate; // Recovery per second when stationary/walking

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float JumpStaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float VaultStaminaCost;

	UPROPERTY(ReplicatedUsing = OnRep_Exhausted, BlueprintReadOnly, Category = "Stamina")
	bool bIsExhausted;

	UFUNCTION()
	void OnRep_Stamina();

	UFUNCTION()
	void OnRep_Exhausted();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetStaminaPercent() const { return MaxStamina > 0.0f ? CurrentStamina / MaxStamina : 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool IsExhausted() const { return bIsExhausted; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Stamina")
	bool ConsumeStamina(float Amount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Stamina")
	void ConsumeSprint(float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Stamina")
	bool CanSprint() const { return !bIsExhausted && CurrentStamina > 5.0f; }

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStaminaChangedSignature OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStaminaExhaustedSignature OnStaminaExhausted;
};

