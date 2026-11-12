// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "BTTask_EngageTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LastZone/AI/LastZoneAICharacter.h"

UBTTask_EngageTarget::UBTTask_EngageTarget()
{
	NodeName = TEXT("Engage Target Enemy");
}

EBTNodeResult::Type UBTTask_EngageTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	ALastZoneAICharacter* AIChar = Cast<ALastZoneAICharacter>(AICon->GetPawn());
	if (!AIChar) return EBTNodeResult::Failed;

	AActor* TargetEnemy = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName(TEXT("TargetEnemy"))));
	if (!TargetEnemy) return EBTNodeResult::Failed;

	AIChar->BotFireAtTarget(TargetEnemy);
	return EBTNodeResult::Succeeded;
}

