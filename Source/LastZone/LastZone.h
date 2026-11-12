// Copyright 2026 Last Zone Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLastZone, Log, All);

// Collision channels for Last Zone
#define COLLISION_WEAPON_TRACE     ECC_GameTraceChannel1
#define COLLISION_PROJECTILE       ECC_GameTraceChannel2
#define COLLISION_INTERACTION      ECC_GameTraceChannel3
#define COLLISION_SAFEZONE         ECC_GameTraceChannel4

