// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneGrenadeProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Engine/DamageEvents.h"
#include "PhysicsEngine/RadialForceComponent.h"

ALastZoneGrenadeProjectile::ALastZoneGrenadeProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->InitSphereRadius(6.0f);
	SphereComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	SphereComp->SetSimulatePhysics(true);
	SphereComp->SetNotifyRigidBodyCollision(true);
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SphereComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->UpdatedComponent = SphereComp;
	MovementComp->InitialSpeed = 1600.0f;
	MovementComp->MaxSpeed = 2500.0f;
	MovementComp->bShouldBounce = true;
	MovementComp->Bounciness = 0.35f;
	MovementComp->Friction = 0.6f;

	FuseTime = 4.0f;
	ExplosionRadius = 750.0f; // 7.5 meters
	MaxDamage = 110.0f;
	MinDamage = 25.0f;
	ImpulseStrength = 80000.0f;
}

void ALastZoneGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(FuseTimerHandle, this, &ALastZoneGrenadeProjectile::Detonate, FuseTime, false);
	}
}

void ALastZoneGrenadeProjectile::Throw(const FVector& ImpulseVector)
{
	if (SphereComp)
	{
		SphereComp->AddImpulse(ImpulseVector, NAME_None, true);
	}
}

void ALastZoneGrenadeProjectile::Detonate()
{
	if (!HasAuthority()) return;

	MulticastExplosionEffects();

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamageWithFalloff(
		this,
		MaxDamage,
		MinDamage,
		GetActorLocation(),
		ExplosionRadius * 0.25f, // Full damage inner radius
		ExplosionRadius,        // Outer falloff radius
		1.0f,                   // Falloff exponent
		UDamageType::StaticClass(),
		IgnoredActors,
		this,
		GetInstigatorController(),
		ECC_Visibility
	);

	// Physics impulse to Chaos bodies
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation(), FQuat::Identity, ECC_WorldDynamic, SphereShape);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (UPrimitiveComponent* PrimComp = Overlap.GetComponent())
		{
			if (PrimComp->IsSimulatingPhysics())
			{
				PrimComp->AddRadialImpulse(GetActorLocation(), ExplosionRadius, ImpulseStrength, ERadialImpulseFalloff::RIF_Linear, true);
			}
		}
	}

	SetLifeSpan(0.1f);
}

void ALastZoneGrenadeProjectile::MulticastExplosionEffects_Implementation()
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

