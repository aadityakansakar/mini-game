// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LastZone/LastZoneTypes.h"
#include "LastZoneAnimInstance.generated.h"

class ALastZoneCharacter;

UCLASS()
class LASTZONE_API ULastZoneAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	ULastZoneAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	ALastZoneCharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsAccelerating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsCrouched;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsProne;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aiming")
	float AimPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aiming")
	float LeanAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType EquippedWeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bHasWeaponEquipped;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead;
};

