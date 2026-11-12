// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_EvaluateTactics.generated.h"

UCLASS()
class LASTZONE_API UBTService_EvaluateTactics : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_EvaluateTactics();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

