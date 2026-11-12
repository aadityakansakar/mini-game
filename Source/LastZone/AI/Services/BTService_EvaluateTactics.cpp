// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "BTService_EvaluateTactics.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "LastZone/Character/LastZoneCharacter.h"
#include "LastZone/Character/Components/HealthComponent.h"
#include "LastZone/Character/Components/InventoryComponent.h"
#include "LastZone/Weapons/LastZoneWeaponBase.h"
#include "LastZone/Zone/SafeZoneManager.h"

UBTService_EvaluateTactics::UBTService_EvaluateTactics()
{
	NodeName = TEXT("Evaluate AI Tactics");
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_EvaluateTactics::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon || !AICon->GetPawn()) return;

	ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(AICon->GetPawn());
	if (!Char) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	// 1. Health evaluation
	if (Char->GetHealthComponent())
	{
		float HealthPercent = Char->GetHealthComponent()->GetHealthPercent();
		BB->SetValueAsBool(FName(TEXT("bIsLowHealth")), HealthPercent < 0.40f);
	}

	// 2. Ammo & Reload evaluation
	if (Char->GetInventoryComponent())
	{
		ALastZoneWeaponBase* Weapon = Char->GetInventoryComponent()->GetCurrentWeapon();
		bool bHasWeapon = (Weapon != nullptr);
		BB->SetValueAsBool(FName(TEXT("bHasWeapon")), bHasWeapon);

		if (bHasWeapon && Weapon->GetCurrentAmmo() == 0 && Weapon->GetReserveAmmo() > 0)
		{
			Weapon->StartReload();
		}
	}

	// 3. Safe Zone urgency evaluation
	for (TActorIterator<ASafeZoneManager> It(Char->GetWorld()); It; ++It)
	{
		ASafeZoneManager* Zone = *It;
		if (Zone)
		{
			bool bInZone = Zone->IsInsideCurrentZone(Char->GetActorLocation());
			BB->SetValueAsBool(FName(TEXT("bOutsideSafeZone")), !bInZone);
		}
		break;
	}
}

