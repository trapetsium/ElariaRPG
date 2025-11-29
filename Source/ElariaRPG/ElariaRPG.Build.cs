using UnrealBuildTool;

public class ElariaRPG : ModuleRules
{
    public ElariaRPG(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "AIModule",
            "GameplayTasks",
            "UMG"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}