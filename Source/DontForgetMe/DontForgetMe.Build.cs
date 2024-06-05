
using UnrealBuildTool;

public class DontForgetMe : ModuleRules
{
	public DontForgetMe(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "Slate", "SlateCore", "Niagara" });

        PrivateDependencyModuleNames.AddRange(new string[] {  });

	}
}
