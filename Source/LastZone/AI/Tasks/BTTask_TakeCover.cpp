// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "BTTask_TakeCover.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "LastZone/Character/LastZoneCharacter.h"

UBTTask_TakeCover::UBTTask_TakeCover()
{
	NodeName = TEXT("Take Cover From Enemy");
}

EBTNodeResult::Type UBTTask_TakeCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon || !AICon->GetPawn()) return EBTNodeResult::Failed;

	APawn* AIPawn = AICon->GetPawn();
	AActor* Enemy = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName(TEXT("TargetEnemy"))));
	if (!Enemy) return EBTNodeResult::Failed;

	FVector EnemyLoc = Enemy->GetActorLocation();
	FVector PawnLoc = AIPawn->GetActorLocation();
	FVector AwayDir = (PawnLoc - EnemyLoc).GetSafeNormal2D();

	// Search 8 directions around pawn for a position with blocked line of sight to enemy
	UWorld* World = AIPawn->GetWorld();
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(AIPawn);
	TraceParams.AddIgnoredActor(Enemy);

	for (int32 i = 0; i < 8; ++i)
	{
		float Angle = (2.0f * PI / 8.0f) * i;
		FVector CandidateOffset = FVector(FMath::Cos(Angle) * 600.0f, FMath::Sin(Angle) * 600.0f, 0.0f);
		FVector CandidateLoc = PawnLoc + CandidateOffset;

		// Check if point blocks line of sight to enemy
		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, CandidateLoc + FVector(0,0,50), EnemyLoc + FVector(0,0,50), ECC_WorldStatic, TraceParams))
		{
			// Line of sight is blocked by obstacle! Move here.
			if (NavSys)
			{
				FNavLocation NavLoc;
				if (NavSys->ProjectPointToNavigation(CandidateLoc, NavLoc, FVector(200, 200, 500)))
				{
					AICon->MoveToLocation(NavLoc.Location, 50.0f);

					if (ALastZoneCharacter* Char = Cast<ALastZoneCharacter>(AIPawn))
					{
						Char->ToggleCrouch();
					}
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}

