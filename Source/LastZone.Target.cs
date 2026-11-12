// Copyright 2026 Last Zone Studios. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class LastZoneTarget : TargetRules
{
	public LastZoneTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.AddRange(new string[] { "LastZone" });
	}
}

