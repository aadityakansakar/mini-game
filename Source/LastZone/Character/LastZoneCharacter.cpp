// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/HealthComponent.h"
#include "Components/ArmorComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/InventoryComponent.h"
#include "LastZone/Weapons/LastZoneWeaponBase.h"
#include "LastZone/Vehicles/LastZoneVehicleBase.h"
#include "LastZone/Loot/GroundItemPickup.h"
#include "LastZone/Core/LastZonePlayerController.h"
#include "LastZone/LastZone.h"

ALastZoneCharacter::ALastZoneCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// Capsule configuration for human military character
	GetCapsuleComponent()->InitCapsuleSize(38.0f, 90.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Character Movement setup
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 420.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 450.0f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// Speeds
	WalkSpeed = 280.0f;
	JogSpeed = 450.0f;
	SprintSpeed = 640.0f;
	CrouchSpeed = 220.0f;
	ProneSpeed = 95.0f;
	SwimSpeed = 220.0f;

	// Camera Boom & Follow Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 260.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 15.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 55.0f, 35.0f); // Over-the-shoulder right

	DefaultCameraSocketOffset = FVector(0.0f, 55.0f, 35.0f);
	ADSCameraSocketOffset = FVector(40.0f, 25.0f, 15.0f);
	DefaultFOV = 90.0f;
	CurrentADSAlpha = 0.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetFieldOfView(DefaultFOV);

	// Gameplay Components
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	ArmorComponent = CreateDefaultSubobject<UArmorComponent>(TEXT("ArmorComponent"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// Stance & Movement defaults
	PlayerStance = EPlayerStance::Standing;
	PlayerMovementState = EPlayerMovementState::Idle;
	bIsAiming = false;
	AimPitch = 0.0f;

	CurrentVehicle = nullptr;
	CurrentSeatIndex = -1;

	VaultTraceDistance = 120.0f;
	MaxVaultHeight = 160.0f;
	bIsVaulting = false;
	VaultTimelineProgress = 0.0f;

	TargetRecoilPitch = 0.0f;
	TargetRecoilYaw = 0.0f;
	AccumulatedRecoilPitch = 0.0f;
	AccumulatedRecoilYaw = 0.0f;
}

void ALastZoneCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Enhanced Input mapping context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// Register death and downed delegates
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ALastZoneCharacter::HandleCharacterDeath);
		HealthComponent->OnDowned.AddDynamic(this, &ALastZoneCharacter::HandleCharacterDowned);
	}
}

void ALastZoneCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1. Aim Down Sights (ADS) Camera Interpolation
	float TargetAlpha = bIsAiming ? 1.0f : 0.0f;
	CurrentADSAlpha = FMath::FInterpTo(CurrentADSAlpha, TargetAlpha, DeltaTime, 12.0f);

	if (CameraBoom && FollowCamera)
	{
		CameraBoom->SocketOffset = FMath::Lerp(DefaultCameraSocketOffset, ADSCameraSocketOffset, CurrentADSAlpha);
		float TargetFOV = DefaultFOV;
		if (bIsAiming && InventoryComponent && InventoryComponent->GetCurrentWeapon())
		{
			TargetFOV = InventoryComponent->GetCurrentWeapon()->GetStats().AimDownSightsFOV;
		}
		FollowCamera->SetFieldOfView(FMath::Lerp(DefaultFOV, TargetFOV, CurrentADSAlpha));
	}

	// 2. Sprint Stamina & Movement State logic
	if (HasAuthority())
	{
		FVector Velocity2D = GetVelocity();
		Velocity2D.Z = 0.0f;
		float CurrentSpeed = Velocity2D.Size();

		if (GetCharacterMovement()->IsSwimming())
		{
			PlayerMovementState = EPlayerMovementState::Swimming;
			GetCharacterMovement()->MaxSwimSpeed = SwimSpeed;
		}
		else if (GetCharacterMovement()->IsFalling())
		{
			PlayerMovementState = EPlayerMovementState::Falling;
		}
		else if (PlayerMovementState == EPlayerMovementState::Sprinting)
		{
			if (CurrentSpeed > 10.0f && StaminaComponent)
			{
				StaminaComponent->ConsumeSprint(DeltaTime);
				if (!StaminaComponent->CanSprint())
				{
					StopSprint();
				}
			}
			else
			{
				StopSprint();
			}
		}
		else
		{
			if (CurrentSpeed < 10.0f)
			{
				PlayerMovementState = EPlayerMovementState::Idle;
			}
			else
			{
				PlayerMovementState = EPlayerMovementState::Walking;
			}
		}

		// Replicate Aim Pitch
		FRotator ControlRot = GetControlRotation();
		AimPitch = FRotator::NormalizeAxis(ControlRot.Pitch);
	}

	// 3. Vaulting Smooth Translation
	if (bIsVaulting)
	{
		VaultTimelineProgress += DeltaTime * 3.0f; // 0.33s vault duration
		if (VaultTimelineProgress >= 1.0f)
		{
			bIsVaulting = false;
			SetActorLocation(VaultEndPos);
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		else
		{
			FVector CurrentPos = FMath::Lerp(VaultStartPos, VaultEndPos, VaultTimelineProgress);
			// Arc curve for vaulting over obstacle
			CurrentPos.Z += FMath::Sin(VaultTimelineProgress * PI) * 30.0f;
			SetActorLocation(CurrentPos);
		}
	}

	// 4. Smooth Recoil Recovery
	if (IsLocallyControlled())
	{
		if (FMath::Abs(TargetRecoilPitch) > 0.01f || FMath::Abs(TargetRecoilYaw) > 0.01f)
		{
			float StepPitch = FMath::FInterpTo(0.0f, TargetRecoilPitch, DeltaTime, 10.0f);
			float StepYaw = FMath::FInterpTo(0.0f, TargetRecoilYaw, DeltaTime, 10.0f);

			AddControllerPitchInput(-StepPitch);
			AddControllerYawInput(StepYaw);

			TargetRecoilPitch -= StepPitch;
			TargetRecoilYaw -= StepYaw;
		}
	}
}

void ALastZoneCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement & Camera
		if (MoveAction) EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALastZoneCharacter::Move);
		if (LookAction) EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALastZoneCharacter::Look);
		if (JumpAction) EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		if (JumpAction) EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Stances & Sprint
		if (SprintAction) EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ALastZoneCharacter::StartSprint);
		if (SprintAction) EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ALastZoneCharacter::StopSprint);
		if (CrouchAction) EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ALastZoneCharacter::ToggleCrouch);
		if (ProneAction) EnhancedInputComponent->BindAction(ProneAction, ETriggerEvent::Started, this, &ALastZoneCharacter::ToggleProne);

		// Weapons & Combat
		if (FireAction) EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ALastZoneCharacter::StartFire);
		if (FireAction) EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ALastZoneCharacter::StopFire);
		if (AimAction) EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ALastZoneCharacter::StartAim);
		if (AimAction) EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ALastZoneCharacter::StopAim);
		if (ReloadAction) EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ALastZoneCharacter::Reload);

		// Interaction & Inventory
		if (InteractAction) EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ALastZoneCharacter::Interact);
		if (SwitchWeapon1Action) EnhancedInputComponent->BindAction(SwitchWeapon1Action, ETriggerEvent::Started, this, &ALastZoneCharacter::SelectPrimaryWeapon);
		if (SwitchWeapon2Action) EnhancedInputComponent->BindAction(SwitchWeapon2Action, ETriggerEvent::Started, this, &ALastZoneCharacter::SelectSecondaryWeapon);
		if (SwitchSidearmAction) EnhancedInputComponent->BindAction(SwitchSidearmAction, ETriggerEvent::Started, this, &ALastZoneCharacter::SelectSidearm);
		if (ToggleInventoryAction) EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ALastZoneCharacter::ToggleInventory);
	}
}

void ALastZoneCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr && !bIsVaulting)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALastZoneCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ALastZoneCharacter::StartSprint()
{
	if (PlayerStance != EPlayerStance::Standing)
	{
		ServerSetStance(EPlayerStance::Standing);
	}

	if (StaminaComponent && StaminaComponent->CanSprint())
	{
		PlayerMovementState = EPlayerMovementState::Sprinting;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ALastZoneCharacter::StopSprint()
{
	if (PlayerMovementState == EPlayerMovementState::Sprinting)
	{
		PlayerMovementState = EPlayerMovementState::Walking;
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	}
}

void ALastZoneCharacter::ToggleCrouch()
{
	if (PlayerStance == EPlayerStance::Crouching)
	{
		ServerSetStance(EPlayerStance::Standing);
	}
	else
	{
		ServerSetStance(EPlayerStance::Crouching);
	}
}

void ALastZoneCharacter::ToggleProne()
{
	if (PlayerStance == EPlayerStance::Prone)
	{
		ServerSetStance(EPlayerStance::Standing);
	}
	else
	{
		ServerSetStance(EPlayerStance::Prone);
	}
}

void ALastZoneCharacter::StartFire()
{
	if (InventoryComponent && InventoryComponent->GetCurrentWeapon())
	{
		InventoryComponent->GetCurrentWeapon()->StartFire();
	}
}

void ALastZoneCharacter::StopFire()
{
	if (InventoryComponent && InventoryComponent->GetCurrentWeapon())
	{
		InventoryComponent->GetCurrentWeapon()->StopFire();
	}
}

void ALastZoneCharacter::StartAim()
{
	bIsAiming = true;
	ServerSetAiming(true);
}

void ALastZoneCharacter::StopAim()
{
	bIsAiming = false;
	ServerSetAiming(false);
}

void ALastZoneCharacter::Reload()
{
	if (InventoryComponent && InventoryComponent->GetCurrentWeapon())
	{
		InventoryComponent->GetCurrentWeapon()->StartReload();
	}
}

void ALastZoneCharacter::Interact()
{
	ServerInteractWithObject();
}

void ALastZoneCharacter::SelectPrimaryWeapon()
{
	if (InventoryComponent) InventoryComponent->ServerSwitchSlot(0);
}

void ALastZoneCharacter::SelectSecondaryWeapon()
{
	if (InventoryComponent) InventoryComponent->ServerSwitchSlot(1);
}

void ALastZoneCharacter::SelectSidearm()
{
	if (InventoryComponent) InventoryComponent->ServerSwitchSlot(2);
}

void ALastZoneCharacter::ToggleInventory()
{
	if (ALastZonePlayerController* PC = Cast<ALastZonePlayerController>(GetController()))
	{
		PC->ToggleInventoryScreen();
	}
}

void ALastZoneCharacter::ServerSetStance_Implementation(EPlayerStance NewStance)
{
	PlayerStance = NewStance;
	OnRep_PlayerStance();
}

void ALastZoneCharacter::ServerSetAiming_Implementation(bool bNewAiming)
{
	bIsAiming = bNewAiming;
	OnRep_IsAiming();
}

void ALastZoneCharacter::ServerSetAimPitch_Implementation(float NewPitch)
{
	AimPitch = NewPitch;
}

void ALastZoneCharacter::ServerTryVault_Implementation()
{
	FVector Landing;
	if (CheckVaultObstacle(Landing))
	{
		ExecuteVault(Landing);
	}
}

bool ALastZoneCharacter::CheckVaultObstacle(FVector& OutVaultLanding)
{
	UWorld* World = GetWorld();
	if (!World) return false;

	FVector Forward = GetActorForwardVector();
	FVector StartTrace = GetActorLocation() + FVector(0.0f, 0.0f, 20.0f);
	FVector EndTrace = StartTrace + (Forward * VaultTraceDistance);

	FHitResult ForwardHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (World->LineTraceSingleByChannel(ForwardHit, StartTrace, EndTrace, ECC_WorldStatic, Params))
	{
		// Downward trace from above obstacle
		FVector DownStart = ForwardHit.ImpactPoint + (Forward * 30.0f) + FVector(0.0f, 0.0f, MaxVaultHeight);
		FVector DownEnd = DownStart - FVector(0.0f, 0.0f, MaxVaultHeight + 50.0f);

		FHitResult DownHit;
		if (World->LineTraceSingleByChannel(DownHit, DownStart, DownEnd, ECC_WorldStatic, Params))
		{
			float ObstacleHeight = DownHit.ImpactPoint.Z - GetActorLocation().Z;
			if (ObstacleHeight > 30.0f && ObstacleHeight <= MaxVaultHeight)
			{
				OutVaultLanding = DownHit.ImpactPoint + FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 2.0f);
				return true;
			}
		}
	}
	return false;
}

void ALastZoneCharacter::ExecuteVault(const FVector& LandingPos)
{
	bIsVaulting = true;
	VaultTimelineProgress = 0.0f;
	VaultStartPos = GetActorLocation();
	VaultEndPos = LandingPos;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	if (StaminaComponent)
	{
		StaminaComponent->ConsumeStamina(10.0f);
	}
}

void ALastZoneCharacter::ServerInteractWithObject_Implementation()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FVector CameraLocation = FollowCamera ? FollowCamera->GetComponentLocation() : GetActorLocation();
	FVector CameraForward = GetControlRotation().Vector();
	FVector TraceEnd = CameraLocation + (CameraForward * 300.0f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (World->LineTraceSingleByChannel(Hit, CameraLocation, TraceEnd, ECC_Visibility, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (AGroundItemPickup* Pickup = Cast<AGroundItemPickup>(HitActor))
		{
			if (InventoryComponent && InventoryComponent->AddItem(Pickup->GetItemData()))
			{
				Pickup->Destroy();
			}
		}
		else if (ALastZoneVehicleBase* Vehicle = Cast<ALastZoneVehicleBase>(HitActor))
		{
			EnterVehicle(Vehicle, 0); // Enter driver seat
		}
	}
}

void ALastZoneCharacter::EnterVehicle(ALastZoneVehicleBase* Vehicle, int32 SeatIndex)
{
	if (!Vehicle) return;

	CurrentVehicle = Vehicle;
	CurrentSeatIndex = SeatIndex;
	PlayerMovementState = EPlayerMovementState::Driving;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorHiddenInGame(true);
	AttachToActor(Vehicle, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ALastZoneCharacter::ExitVehicle()
{
	if (!CurrentVehicle) return;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorHiddenInGame(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	FVector ExitLocation = CurrentVehicle->GetActorLocation() + (CurrentVehicle->GetActorRightVector() * -180.0f) + FVector(0.0f, 0.0f, 50.0f);
	SetActorLocation(ExitLocation);

	CurrentVehicle = nullptr;
	CurrentSeatIndex = -1;
	PlayerMovementState = EPlayerMovementState::Walking;
}

void ALastZoneCharacter::AddRecoilInput(float PitchRecoil, float YawRecoil)
{
	TargetRecoilPitch += PitchRecoil;
	TargetRecoilYaw += YawRecoil;
}

void ALastZoneCharacter::OnRep_PlayerStance()
{
	switch (PlayerStance)
	{
	case EPlayerStance::Standing:
		GetCapsuleComponent()->SetCapsuleSize(38.0f, 90.0f);
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
		break;
	case EPlayerStance::Crouching:
		GetCapsuleComponent()->SetCapsuleSize(38.0f, 60.0f);
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
		break;
	case EPlayerStance::Prone:
		GetCapsuleComponent()->SetCapsuleSize(35.0f, 25.0f);
		GetCharacterMovement()->MaxWalkSpeed = ProneSpeed;
		break;
	}
}

void ALastZoneCharacter::OnRep_MovementState()
{
	// Client movement effects & animation state
}

void ALastZoneCharacter::OnRep_IsAiming()
{
	// Client aim updates
}

void ALastZoneCharacter::OnRep_CurrentVehicle()
{
	if (CurrentVehicle)
	{
		SetActorHiddenInGame(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		SetActorHiddenInGame(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ALastZoneCharacter::HandleCharacterDeath(AActor* Killer, AActor* Victim)
{
	PlayerMovementState = EPlayerMovementState::Ragdoll;
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Enable full Chaos physical ragdoll on character mesh
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp)
	{
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComp->SetSimulatePhysics(true);
		MeshComp->WakeAllRigidBodies();
		MeshComp->bBlendPhysics = true;
	}

	// Drop equipped weapon onto ground
	if (InventoryComponent)
	{
		InventoryComponent->HolsterWeapon();
	}
}

void ALastZoneCharacter::HandleCharacterDowned(AActor* DownedActor)
{
	ServerSetStance(EPlayerStance::Crouching);
	GetCharacterMovement()->MaxWalkSpeed = 70.0f; // Crawling speed
}

void ALastZoneCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALastZoneCharacter, PlayerStance);
	DOREPLIFETIME(ALastZoneCharacter, PlayerMovementState);
	DOREPLIFETIME(ALastZoneCharacter, bIsAiming);
	DOREPLIFETIME(ALastZoneCharacter, AimPitch);
	DOREPLIFETIME(ALastZoneCharacter, CurrentVehicle);
	DOREPLIFETIME(ALastZoneCharacter, CurrentSeatIndex);
}

