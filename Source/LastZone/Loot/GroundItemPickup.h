// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LastZone/LastZoneTypes.h"
#include "GroundItemPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

UCLASS()
class LASTZONE_API AGroundItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	AGroundItemPickup();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* RarityBeamFX;

	UPROPERTY(ReplicatedUsing = OnRep_ItemData, EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FLastZoneItem ItemData;

	UFUNCTION()
	void OnRep_ItemData();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Loot")
	void InitializeItem(const FLastZoneItem& InItem);

	UFUNCTION(BlueprintCallable, Category = "Loot")
	const FLastZoneItem& GetItemData() const { return ItemData; }

	FLinearColor GetRarityColor() const;
};

