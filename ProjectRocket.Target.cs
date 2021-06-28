// Copyright (c) 2020 Tension Graphics AB

using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectRocketTarget : TargetRules
{
	public ProjectRocketTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "ProjectRocket" } );
	}
}
