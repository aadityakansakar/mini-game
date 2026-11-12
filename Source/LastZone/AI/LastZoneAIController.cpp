// Copyright 2026 Last Zone Studios. All Rights Reserved.

#include "LastZoneAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "LastZone/Character/LastZoneCharacter.h"
#include "LastZone/Character/Components/HealthComponent.h"

ALastZoneAIController::ALastZoneAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));

	// Sight Configuration (Realistic Battle Royale vision range)
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 4500.0f; // 45 meters
	SightConfig->LoseSightRadius = 5500.0f;
	SightConfig->PeripheralVisionAngleDegrees = 85.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->SetMaxAge(5.0f);

	// Hearing Configuration (Gunfire & footsteps detection)
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 3500.0f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->SetMaxAge(3.0f);

	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->ConfigureSense(*HearingConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ALastZoneAIController::HandleTargetPerceptionUpdated);
}

void ALastZoneAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ALastZoneAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void ALastZoneAIController::HandlePerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	// Process broad perception batch
}

void ALastZoneAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	ALastZoneCharacter* PerceivedChar = Cast<ALastZoneCharacter>(Actor);
	if (!PerceivedChar || PerceivedChar == GetPawn()) return;

	// Ignore dead characters
	if (PerceivedChar->GetHealthComponent() && PerceivedChar->GetHealthComponent()->IsDead())
	{
		if (GetTargetEnemy() == PerceivedChar)
		{
			ClearTargetEnemy();
		}
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		// Target acquired
		SetTargetEnemy(PerceivedChar);
	}
	else
	{
		// Target lost sight
		if (GetTargetEnemy() == PerceivedChar)
		{
			ClearTargetEnemy();
		}
	}
}

void ALastZoneAIController::SetTargetEnemy(AActor* NewTarget)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject(FName(TEXT("TargetEnemy")), NewTarget);
	}
}

AActor* ALastZoneAIController::GetTargetEnemy() const
{
	if (BlackboardComponent)
	{
		return Cast<AActor>(BlackboardComponent->GetValueAsObject(FName(TEXT("TargetEnemy"))));
	}
	return nullptr;
}

void ALastZoneAIController::ClearTargetEnemy()
{
	if (BlackboardComponent)
	{
		BlackboardComponent->ClearValue(FName(TEXT("TargetEnemy")));
	}
}

