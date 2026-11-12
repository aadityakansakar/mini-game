// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneWeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "LastZone/Character/LastZoneCharacter.h"
#include "LastZone/LastZone.h"

ALastZoneWeaponBase::ALastZoneWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
	RootComponent = WeaponMesh;

	CurrentAmmoInClip = 30;
	ReserveAmmo = 120;
	bIsReloading = false;
	bIsFiring = false;
}

void ALastZoneWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmoInClip = WeaponStats.MagazineCapacity;
}

bool ALastZoneWeaponBase::CanFire() const
{
	return !bIsReloading && CurrentAmmoInClip > 0;
}

bool ALastZoneWeaponBase::CanReload() const
{
	return !bIsReloading && CurrentAmmoInClip < WeaponStats.MagazineCapacity && ReserveAmmo > 0;
}

void ALastZoneWeaponBase::StartFire()
{
	bIsFiring = true;
	ServerStartFire();

	if (CanFire())
	{
		FireShot();

		if (WeaponStats.FireRateRPM > 0.0f)
		{
			float TimeBetweenShots = 60.0f / WeaponStats.FireRateRPM;
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ALastZoneWeaponBase::FireShot, TimeBetweenShots, true);
		}
	}
	else if (CurrentAmmoInClip <= 0)
	{
		if (EmptyClickSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EmptyClickSound, GetActorLocation());
		}
		StartReload();
	}
}

void ALastZoneWeaponBase::StopFire()
{
	bIsFiring = false;
	ServerStopFire();
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ALastZoneWeaponBase::FireShot()
{
	if (!CanFire())
	{
		StopFire();
		return;
	}

	// Apply recoil to character controller if locally controlled
	if (ALastZoneCharacter* Character = Cast<ALastZoneCharacter>(GetOwner()))
	{
		float PitchRecoil = FMath::RandRange(WeaponStats.RecoilPitchMin, WeaponStats.RecoilPitchMax);
		float YawRecoil = FMath::RandRange(WeaponStats.RecoilYawMin, WeaponStats.RecoilYawMax);
		Character->AddRecoilInput(PitchRecoil, YawRecoil);

		FVector MuzzleLoc = GetMuzzleLocation();
		FVector SpreadForward = CalculateSpreadVector(Character->GetControlRotation().Vector(), Character->IsAiming(), Character->GetPlayerStance(), Character->GetVelocity().Size() > 10.0f);
		ExecuteFireLogic(MuzzleLoc, SpreadForward.Rotation());
	}

	MulticastPlayFireEffects();

	if (HasAuthority())
	{
		CurrentAmmoInClip = FMath::Clamp(CurrentAmmoInClip - 1, 0, WeaponStats.MagazineCapacity);
		OnAmmoChanged.Broadcast(CurrentAmmoInClip, ReserveAmmo);
	}
}

void ALastZoneWeaponBase::StartReload()
{
	if (!CanReload()) return;

	bIsReloading = true;
	ServerReload();

	OnReloadStateChanged.Broadcast(true);
	MulticastPlayReloadEffects();

	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &ALastZoneWeaponBase::FinishReload, WeaponStats.ReloadTimeSeconds, false);
}

void ALastZoneWeaponBase::FinishReload()
{
	bIsReloading = false;
	OnReloadStateChanged.Broadcast(false);

	if (HasAuthority())
	{
		int32 NeededAmmo = WeaponStats.MagazineCapacity - CurrentAmmoInClip;
		int32 AmmoToLoad = FMath::Min(NeededAmmo, ReserveAmmo);

		CurrentAmmoInClip += AmmoToLoad;
		ReserveAmmo -= AmmoToLoad;

		OnAmmoChanged.Broadcast(CurrentAmmoInClip, ReserveAmmo);
	}
}

void ALastZoneWeaponBase::CancelReload()
{
	if (bIsReloading)
	{
		bIsReloading = false;
		GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
		OnReloadStateChanged.Broadcast(false);
	}
}

void ALastZoneWeaponBase::ServerStartFire_Implementation()
{
	bIsFiring = true;
}

bool ALastZoneWeaponBase::ServerStartFire_Validate()
{
	return true;
}

void ALastZoneWeaponBase::ServerStopFire_Implementation()
{
	bIsFiring = false;
}

void ALastZoneWeaponBase::ServerReload_Implementation()
{
	if (CanReload())
	{
		StartReload();
	}
}

bool ALastZoneWeaponBase::ServerReload_Validate()
{
	return true;
}

void ALastZoneWeaponBase::MulticastPlayFireEffects_Implementation()
{
	if (MuzzleFlashFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlashFX, WeaponMesh, FName(TEXT("MuzzleSocket")), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}

void ALastZoneWeaponBase::MulticastPlayReloadEffects_Implementation()
{
	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
	}
}

void ALastZoneWeaponBase::ExecuteFireLogic(const FVector& MuzzleLocation, const FRotator& FireRotation)
{
	// Base implementation override in child classes
}

FVector ALastZoneWeaponBase::CalculateSpreadVector(const FVector& BaseForward, bool bIsAiming, EPlayerStance Stance, bool bIsMoving) const
{
	float Spread = WeaponStats.BaseSpreadAngle;

	// Aim Down Sights reduces spread by 70%
	if (bIsAiming)
	{
		Spread *= 0.3f;
	}

	// Stance modifiers
	switch (Stance)
	{
	case EPlayerStance::Crouching: Spread *= 0.7f; break;
	case EPlayerStance::Prone:     Spread *= 0.4f; break;
	default: break;
	}

	// Movement modifier
	if (bIsMoving)
	{
		Spread *= WeaponStats.MovementSpreadMultiplier;
	}

	// Convert half angle to radians and calculate cone vector
	float ConeHalfAngleRad = FMath::DegreesToRadians(Spread * 0.5f);
	return FMath::VRandCone(BaseForward, ConeHalfAngleRad);
}

FVector ALastZoneWeaponBase::GetMuzzleLocation() const
{
	if (WeaponMesh && WeaponMesh->DoesSocketExist(FName(TEXT("MuzzleSocket"))))
	{
		return WeaponMesh->GetSocketLocation(FName(TEXT("MuzzleSocket")));
	}
	return GetActorLocation() + (GetActorForwardVector() * 50.0f);
}

void ALastZoneWeaponBase::OnRep_Ammo()
{
	OnAmmoChanged.Broadcast(CurrentAmmoInClip, ReserveAmmo);
}

void ALastZoneWeaponBase::OnRep_IsReloading()
{
	OnReloadStateChanged.Broadcast(bIsReloading);
}

void ALastZoneWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALastZoneWeaponBase, CurrentAmmoInClip);
	DOREPLIFETIME(ALastZoneWeaponBase, ReserveAmmo);
	DOREPLIFETIME(ALastZoneWeaponBase, bIsReloading);
}

