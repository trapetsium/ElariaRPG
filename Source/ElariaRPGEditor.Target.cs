using UnrealBuildTool;
using System.Collections.Generic;

public class ElariaRPGEditorTarget : TargetRules
{
    public ElariaRPGEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("ElariaRPG");
    }
}