// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "GroundItemPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"

AGroundItemPickup::AGroundItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->InitSphereRadius(120.0f);
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = SphereCollision;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(SphereCollision);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RarityBeamFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RarityBeamFX"));
	RarityBeamFX->SetupAttachment(SphereCollision);
	RarityBeamFX->bAutoActivate = true;
}

void AGroundItemPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AGroundItemPickup::InitializeItem(const FLastZoneItem& InItem)
{
	ItemData = InItem;
	OnRep_ItemData();
}

void AGroundItemPickup::OnRep_ItemData()
{
	// Set beam color based on rarity
	if (RarityBeamFX)
	{
		RarityBeamFX->SetVariableColor(FName(TEXT("BeamColor")), GetRarityColor());
	}
}

FLinearColor AGroundItemPickup::GetRarityColor() const
{
	switch (ItemData.Rarity)
	{
	case EItemRarity::Common:    return FLinearColor(0.8f, 0.8f, 0.8f, 1.0f); // Light Grey
	case EItemRarity::Uncommon:  return FLinearColor(0.1f, 0.9f, 0.2f, 1.0f); // Green
	case EItemRarity::Rare:      return FLinearColor(0.1f, 0.4f, 1.0f, 1.0f); // Blue
	case EItemRarity::Epic:      return FLinearColor(0.7f, 0.1f, 0.9f, 1.0f); // Purple
	case EItemRarity::Legendary: return FLinearColor(1.0f, 0.75f, 0.0f, 1.0f); // Gold
	default:                     return FLinearColor::White;
	}
}

void AGroundItemPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGroundItemPickup, ItemData);
}

