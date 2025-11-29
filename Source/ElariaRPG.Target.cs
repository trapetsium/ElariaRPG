using UnrealBuildTool;
using System.Collections.Generic;

public class ElariaRPGTarget : TargetRules
{
    public ElariaRPGTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("ElariaRPG");
    }
}