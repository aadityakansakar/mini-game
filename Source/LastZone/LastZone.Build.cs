// Copyright 2026 Last Zone Studios. All Rights Reserved.

using UnrealBuildTool;

public class LastZone : ModuleRules
{
	public LastZone(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"ChaosVehicles",
			"PhysicsCore",
			"AIModule",
			"GameplayTasks",
			"NavigationSystem",
			"UMG",
			"Slate",
			"SlateCore",
			"NetCore",
			"Niagara",
			"RenderCore",
			"RHI"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			// Private dependencies
		});

		// Ensure support for Chaos physics & replication
		SetupIrisSupport(Target);
	}
}

