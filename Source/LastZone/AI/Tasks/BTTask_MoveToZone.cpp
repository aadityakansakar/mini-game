// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "BTTask_MoveToZone.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "LastZone/Zone/SafeZoneManager.h"

UBTTask_MoveToZone::UBTTask_MoveToZone()
{
	NodeName = TEXT("Navigate To Safe Zone");
}

EBTNodeResult::Type UBTTask_MoveToZone::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon || !AICon->GetPawn()) return EBTNodeResult::Failed;

	APawn* AIPawn = AICon->GetPawn();
	UWorld* World = AIPawn->GetWorld();

	ASafeZoneManager* ZoneManager = nullptr;
	for (TActorIterator<ASafeZoneManager> It(World); It; ++It)
	{
		ZoneManager = *It;
		break;
	}

	if (ZoneManager)
	{
		FVector2D TargetCenter2D = ZoneManager->GetTargetCenter();
		float TargetRadiusMeters = ZoneManager->GetTargetRadius();

		// Random point within 70% of target radius to be well inside
		FVector2D Offset = FVector2D(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f)).GetSafeNormal() * FMath::FRandRange(0.0f, TargetRadiusMeters * 0.7f);
		FVector TargetLoc3D((TargetCenter2D.X + Offset.X) * 100.0f, (TargetCenter2D.Y + Offset.Y) * 100.0f, AIPawn->GetActorLocation().Z);

		// Project to navigable mesh
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
		if (NavSys)
		{
			FNavLocation NavLoc;
			if (NavSys->ProjectPointToNavigation(TargetLoc3D, NavLoc, FVector(500.0f, 500.0f, 2000.0f)))
			{
				TargetLoc3D = NavLoc.Location;
			}
		}

		OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName(TEXT("MoveToLocation")), TargetLoc3D);
		AICon->MoveToLocation(TargetLoc3D, 200.0f);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

