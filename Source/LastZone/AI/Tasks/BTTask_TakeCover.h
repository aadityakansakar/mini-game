// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TakeCover.generated.h"

UCLASS()
class LASTZONE_API UBTTask_TakeCover : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TakeCover();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

