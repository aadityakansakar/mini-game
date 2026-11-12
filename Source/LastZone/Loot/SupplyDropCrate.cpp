// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "SupplyDropCrate.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GroundItemPickup.h"

ASupplyDropCrate::ASupplyDropCrate()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->InitBoxExtent(FVector(80.0f, 80.0f, 60.0f));
	BoxCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	BoxCollision->SetNotifyRigidBodyCollision(true);
	BoxCollision->OnComponentHit.AddDynamic(this, &ASupplyDropCrate::HandleLandingHit);
	RootComponent = BoxCollision;

	CrateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrateMesh"));
	CrateMesh->SetupAttachment(BoxCollision);
	CrateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ParachuteMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ParachuteMesh"));
	ParachuteMesh->SetupAttachment(BoxCollision);
	ParachuteMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	ParachuteMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SmokeSignalFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeSignalFX"));
	SmokeSignalFX->SetupAttachment(BoxCollision);
	SmokeSignalFX->bAutoActivate = false;

	DescentSpeed = 450.0f; // 4.5 m/s slow gentle descent
	bHasLanded = false;
}

void ASupplyDropCrate::BeginPlay()
{
	Super::BeginPlay();
}

void ASupplyDropCrate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bHasLanded && HasAuthority())
	{
		FVector NewLocation = GetActorLocation() - FVector(0.0f, 0.0f, DescentSpeed * DeltaTime);
		SetActorLocation(NewLocation, true);
	}
}

void ASupplyDropCrate::InitializeSupplyDrop(const TArray<FLastZoneItem>& Contents)
{
	CrateContents = Contents;
}

void ASupplyDropCrate::HandleLandingHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bHasLanded && HasAuthority())
	{
		bHasLanded = true;
		OnRep_HasLanded();

		// Spawn crate contents as ground pickups around crate
		for (int32 i = 0; i < CrateContents.Num(); ++i)
		{
			float Angle = (2.0f * PI / CrateContents.Num()) * i;
			FVector Offset = FVector(FMath::Cos(Angle) * 120.0f, FMath::Sin(Angle) * 120.0f, 20.0f);
			FVector ItemLoc = GetActorLocation() + Offset;

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AGroundItemPickup* Pickup = GetWorld()->SpawnActor<AGroundItemPickup>(AGroundItemPickup::StaticClass(), ItemLoc, FRotator::ZeroRotator, SpawnParams);
			if (Pickup)
			{
				Pickup->InitializeItem(CrateContents[i]);
			}
		}
	}
}

void ASupplyDropCrate::OnRep_HasLanded()
{
	if (bHasLanded)
	{
		// Detach / hide parachute
		if (ParachuteMesh)
		{
			ParachuteMesh->SetVisibility(false);
		}

		// Activate red smoke beacon
		if (SmokeSignalFX)
		{
			SmokeSignalFX->Activate();
		}

		if (LandingThudSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, LandingThudSound, GetActorLocation());
		}
	}
}

void ASupplyDropCrate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASupplyDropCrate, bHasLanded);
	DOREPLIFETIME(ASupplyDropCrate, CrateContents);
}

