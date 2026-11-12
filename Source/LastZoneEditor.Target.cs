// Copyright 2026 Last Zone Studios. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class LastZoneEditorTarget : TargetRules
{
	public LastZoneEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.AddRange(new string[] { "LastZone" });
	}
}

