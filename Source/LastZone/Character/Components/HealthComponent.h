// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChangedSignature, UHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeathSignature, AActor*, Killer, AActor*, Victim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDownedSignature, AActor*, DownedCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRevivedSignature, AActor*, RevivedCharacter);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LASTZONE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_Boost, EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentBoost; // 0 to 100 boost bar (provides passive healing and sprint speed)

	UPROPERTY(ReplicatedUsing = OnRep_IsDowned, BlueprintReadOnly, Category = "Health")
	bool bIsDowned;

	UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadOnly, Category = "Health")
	bool bIsDead;

	UFUNCTION()
	void OnRep_Health(float OldHealth);

	UFUNCTION()
	void OnRep_Boost(float OldBoost);

	UFUNCTION()
	void OnRep_IsDowned();

	UFUNCTION()
	void OnRep_IsDead();

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercent() const { return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetBoost() const { return CurrentBoost; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDowned() const { return bIsDowned; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Health")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Health")
	void AddBoost(float BoostAmount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Health")
	void Revive();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDownedSignature OnDowned;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRevivedSignature OnRevived;
};

