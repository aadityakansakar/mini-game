// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "LastZone/LastZone.h"

ALastZoneProjectile::ALastZoneProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(4.0f);
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->OnComponentHit.AddDynamic(this, &ALastZoneProjectile::OnHit);
	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 88000.0f; // 880 m/s in cm/s
	ProjectileMovement->MaxSpeed = 120000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 1.0f;

	Damage = 35.0f;
	HeadshotMultiplier = 2.0f;
	DamageTypeClass = UDamageType::StaticClass();

	InitialLifeSpan = 5.0f;
}

void ALastZoneProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ALastZoneProjectile::InitializeProjectile(float InDamage, float InHeadshotMultiplier, float InitialSpeed, float GravityScale, TSubclassOf<UDamageType> InDamageType)
{
	Damage = InDamage;
	HeadshotMultiplier = InHeadshotMultiplier;
	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = InitialSpeed * 100.0f; // Convert m/s to cm/s
		ProjectileMovement->MaxSpeed = InitialSpeed * 100.0f;
		ProjectileMovement->ProjectileGravityScale = GravityScale;
		ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
	}
	if (InDamageType)
	{
		DamageTypeClass = InDamageType;
	}
}

void ALastZoneProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor != GetInstigator())
	{
		if (HasAuthority())
		{
			float FinalDamage = Damage;
			// Detect headshot based on hit bone name
			bool bIsHeadshot = (Hit.BoneName == FName(TEXT("head")) || Hit.BoneName == FName(TEXT("neck_01")));
			if (bIsHeadshot)
			{
				FinalDamage *= HeadshotMultiplier;
			}
			else if (Hit.BoneName.ToString().Contains(TEXT("arm")) || Hit.BoneName.ToString().Contains(TEXT("leg")))
			{
				FinalDamage *= 0.75f; // Limb damage reduction
			}

			UGameplayStatics::ApplyPointDamage(OtherActor, FinalDamage, Hit.ImpactNormal, Hit, GetInstigatorController(), this, DamageTypeClass);
		}

		// Play impact effects
		if (Hit.BoneName != NAME_None) // Likely character
		{
			if (ImpactFleshFX)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactFleshFX, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
			if (ImpactFleshSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactFleshSound, Hit.ImpactPoint);
			}
		}
		else
		{
			if (ImpactDefaultFX)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactDefaultFX, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
			if (ImpactDefaultSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactDefaultSound, Hit.ImpactPoint);
			}
		}

		Destroy();
	}
}

void ALastZoneProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALastZoneProjectile, Damage);
	DOREPLIFETIME(ALastZoneProjectile, HeadshotMultiplier);
	DOREPLIFETIME(ALastZoneProjectile, DamageTypeClass);
}

