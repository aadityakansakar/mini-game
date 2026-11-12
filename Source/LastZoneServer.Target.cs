// Copyright 2026 Last Zone Studios. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class LastZoneServerTarget : TargetRules
{
	public LastZoneServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.AddRange(new string[] { "LastZone" });
	}
}

