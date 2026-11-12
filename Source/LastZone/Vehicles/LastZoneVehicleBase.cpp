// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneVehicleBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "LastZone/Character/LastZoneCharacter.h"
#include "LastZone/LastZone.h"

ALastZoneVehicleBase::ALastZoneVehicleBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	VehicleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VehicleMesh"));
	VehicleMesh->SetCollisionProfileName(TEXT("Vehicle"));
	VehicleMesh->SetSimulatePhysics(true);
	VehicleMesh->SetNotifyRigidBodyCollision(true);
	RootComponent = VehicleMesh;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 550.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	MaxHealth = 1200.0f;
	CurrentHealth = MaxHealth;
	MaxFuel = 100.0f;
	CurrentFuel = MaxFuel;
	FuelConsumptionRate = 0.4f; // ~4 minutes continuous driving
	MaxSeats = 4;
	bIsDestroyed = false;

	ThrottleInput = 0.0f;
	SteeringInput = 0.0f;
	BrakeInput = 0.0f;

	SeatedPassengers.SetNumZeroed(MaxSeats);
}

void ALastZoneVehicleBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ALastZoneVehicleBase::HandleTakeAnyDamage);
	}
}

void ALastZoneVehicleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateVehiclePhysics(DeltaTime);

	// Adjust camera arm dynamically based on vehicle speed
	float SpeedKmH = FMath::Abs(GetForwardSpeedKmH());
	float TargetArmLength = FMath::Lerp(500.0f, 750.0f, FMath::Clamp(SpeedKmH / 120.0f, 0.0f, 1.0f));
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetArmLength, DeltaTime, 3.0f);
}

void ALastZoneVehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ALastZoneVehicleBase::SetThrottle(float Value)
{
	ThrottleInput = FMath::Clamp(Value, -1.0f, 1.0f);
	ServerUpdateVehicleInput(ThrottleInput, SteeringInput, BrakeInput);
}

void ALastZoneVehicleBase::SetSteering(float Value)
{
	SteeringInput = FMath::Clamp(Value, -1.0f, 1.0f);
	ServerUpdateVehicleInput(ThrottleInput, SteeringInput, BrakeInput);
}

void ALastZoneVehicleBase::SetBrake(float Value)
{
	BrakeInput = FMath::Clamp(Value, 0.0f, 1.0f);
	ServerUpdateVehicleInput(ThrottleInput, SteeringInput, BrakeInput);
}

void ALastZoneVehicleBase::ServerUpdateVehicleInput_Implementation(float InThrottle, float InSteering, float InBrake)
{
	ThrottleInput = InThrottle;
	SteeringInput = InSteering;
	BrakeInput = InBrake;
}

void ALastZoneVehicleBase::UpdateVehiclePhysics(float DeltaTime)
{
	if (bIsDestroyed || !VehicleMesh) return;

	if (HasAuthority())
	{
		// Fuel depletion while accelerating
		if (FMath::Abs(ThrottleInput) > 0.1f && CurrentFuel > 0.0f)
		{
			CurrentFuel = FMath::Clamp(CurrentFuel - (FuelConsumptionRate * DeltaTime), 0.0f, MaxFuel);
		}

		if (CurrentFuel <= 0.0f)
		{
			ThrottleInput = 0.0f;
		}

		// Driving impulse simulation on Chaos chassis
		if (VehicleMesh->IsSimulatingPhysics())
		{
			float ForwardForce = ThrottleInput * 900000.0f; // Newton force scaled to vehicle mass
			FVector Thrust = GetActorForwardVector() * ForwardForce;
			VehicleMesh->AddForce(Thrust);

			// Steering torque
			float AngularSteer = SteeringInput * 300000.0f * FMath::Sign(GetForwardSpeedKmH());
			VehicleMesh->AddTorqueInDegrees(FVector(0.0f, 0.0f, AngularSteer));

			// Braking drag
			if (BrakeInput > 0.1f)
			{
				FVector OpposingBrake = -VehicleMesh->GetPhysicsLinearVelocity() * 2500.0f * BrakeInput;
				VehicleMesh->AddForce(OpposingBrake);
			}
		}
	}
}

bool ALastZoneVehicleBase::RequestEnter(ALastZoneCharacter* Passenger, int32 PreferredSeat)
{
	if (!Passenger || bIsDestroyed) return false;

	if (PreferredSeat >= 0 && PreferredSeat < MaxSeats && SeatedPassengers[PreferredSeat] == nullptr)
	{
		SeatedPassengers[PreferredSeat] = Passenger;
		Passenger->EnterVehicle(this, PreferredSeat);

		// If driver seat, possess vehicle
		if (PreferredSeat == 0)
		{
			if (AController* PC = Passenger->GetController())
			{
				PC->Possess(this);
			}
		}
		return true;
	}

	// Find first available seat
	for (int32 i = 0; i < MaxSeats; ++i)
	{
		if (SeatedPassengers[i] == nullptr)
		{
			SeatedPassengers[i] = Passenger;
			Passenger->EnterVehicle(this, i);
			if (i == 0)
			{
				if (AController* PC = Passenger->GetController())
				{
					PC->Possess(this);
				}
			}
			return true;
		}
	}

	return false;
}

void ALastZoneVehicleBase::RequestExit(ALastZoneCharacter* Passenger)
{
	if (!Passenger) return;

	for (int32 i = 0; i < MaxSeats; ++i)
	{
		if (SeatedPassengers[i] == Passenger)
		{
			SeatedPassengers[i] = nullptr;
			if (i == 0)
			{
				if (AController* VehController = GetController())
				{
					VehController->Possess(Passenger);
				}
			}
			Passenger->ExitVehicle();
			break;
		}
	}
}

void ALastZoneVehicleBase::ServerExitVehicle_Implementation(ALastZoneCharacter* Passenger)
{
	RequestExit(Passenger);
}

float ALastZoneVehicleBase::GetForwardSpeedKmH() const
{
	if (VehicleMesh)
	{
		FVector Velocity = VehicleMesh->GetPhysicsLinearVelocity();
		float ForwardSpeedCmS = FVector::DotProduct(Velocity, GetActorForwardVector());
		return (ForwardSpeedCmS * 0.036f); // cm/s to km/h
	}
	return 0.0f;
}

void ALastZoneVehicleBase::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDestroyed) return;

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	OnVehicleHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		bIsDestroyed = true;
		OnRep_IsDestroyed();

		// Deal fatal damage to seated passengers and eject them
		for (int32 i = 0; i < MaxSeats; ++i)
		{
			if (ALastZoneCharacter* Passenger = SeatedPassengers[i])
			{
				UGameplayStatics::ApplyDamage(Passenger, 300.0f, InstigatedBy, this, UDamageType::StaticClass());
				RequestExit(Passenger);
			}
		}

		// Radial explosion damage
		UGameplayStatics::ApplyRadialDamage(this, 150.0f, GetActorLocation(), 800.0f, UDamageType::StaticClass(), TArray<AActor*>(), this, InstigatedBy);

		OnVehicleDestroyed.Broadcast(this);
	}
}

void ALastZoneVehicleBase::OnRep_Health()
{
	OnVehicleHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void ALastZoneVehicleBase::OnRep_IsDestroyed()
{
	if (ExplosionFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionFX, GetActorLocation(), FRotator::ZeroRotator);
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
}

void ALastZoneVehicleBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALastZoneVehicleBase, CurrentHealth);
	DOREPLIFETIME(ALastZoneVehicleBase, CurrentFuel);
	DOREPLIFETIME(ALastZoneVehicleBase, ThrottleInput);
	DOREPLIFETIME(ALastZoneVehicleBase, SteeringInput);
	DOREPLIFETIME(ALastZoneVehicleBase, BrakeInput);
	DOREPLIFETIME(ALastZoneVehicleBase, bIsDestroyed);
	DOREPLIFETIME(ALastZoneVehicleBase, SeatedPassengers);
}

