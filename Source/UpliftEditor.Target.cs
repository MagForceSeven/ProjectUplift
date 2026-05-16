// Copyright Russell Aasland. All Rights Reserved.

using UnrealBuildTool;

public class UpliftEditorTarget : TargetRules
{
	public UpliftEditorTarget(TargetInfo Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.Latest;
		NativePointerMemberBehaviorOverride = PointerMemberBehavior.Disallow;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
 
        Type = TargetType.Editor;

        ExtraModuleNames.Add("Uplift");
		ExtraModuleNames.Add("UpliftDeveloper");
		ExtraModuleNames.Add("UpliftEditor");
        
        ProjectDefinitions.Add("STARFIRE_BUILD_RTM=0");
	}
}
