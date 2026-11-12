// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToZone.generated.h"

UCLASS()
class LASTZONE_API UBTTask_MoveToZone : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToZone();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

