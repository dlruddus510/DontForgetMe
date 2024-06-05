
using UnrealBuildTool;
using System.Collections.Generic;

public class DontForgetMeTarget : TargetRules
{
	public DontForgetMeTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "DontForgetMe" } );
	}
}
