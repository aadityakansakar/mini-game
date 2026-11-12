// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "BTTask_FindLoot.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "LastZone/Loot/GroundItemPickup.h"

UBTTask_FindLoot::UBTTask_FindLoot()
{
	NodeName = TEXT("Find Nearest Loot");
}

EBTNodeResult::Type UBTTask_FindLoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon || !AICon->GetPawn()) return EBTNodeResult::Failed;

	APawn* AIPawn = AICon->GetPawn();
	FVector PawnLoc = AIPawn->GetActorLocation();

	AGroundItemPickup* NearestPickup = nullptr;
	float ClosestDistSq = MAX_FLT;

	for (TActorIterator<AGroundItemPickup> It(AIPawn->GetWorld()); It; ++It)
	{
		AGroundItemPickup* Pickup = *It;
		if (Pickup && !Pickup->IsPendingKillPending())
		{
			float DistSq = FVector::DistSquared(PawnLoc, Pickup->GetActorLocation());
			if (DistSq < ClosestDistSq && DistSq < (3000.0f * 3000.0f)) // within 30m
			{
				ClosestDistSq = DistSq;
				NearestPickup = Pickup;
			}
		}
	}

	if (NearestPickup)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName(TEXT("TargetLoot")), NearestPickup);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName(TEXT("MoveToLocation")), NearestPickup->GetActorLocation());
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

