// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "LastZoneAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class ALastZoneCharacter;

UCLASS()
class LASTZONE_API ALastZoneAIController : public AAIController
{
	GENERATED_BODY()

public:
	ALastZoneAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;

	UFUNCTION()
	void HandlePerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargetEnemy(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetEnemy() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void ClearTargetEnemy();
};

