// Copyright (c) 2024, Nardoritos. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class AuraEditorTarget : TargetRules
{
	public AuraEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "Aura" } );
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
	}
}
