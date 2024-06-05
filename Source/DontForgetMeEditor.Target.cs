
using UnrealBuildTool;
using System.Collections.Generic;

public class DontForgetMeEditorTarget : TargetRules
{
	public DontForgetMeEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "DontForgetMe" } );
	}
}
