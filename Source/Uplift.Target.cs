// Copyright Russell Aasland. All Rights Reserved.

using UnrealBuildTool;

public class UpliftTarget : TargetRules
{
	public UpliftTarget(TargetInfo Target) : base(Target)
    {
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		NativePointerMemberBehaviorOverride = PointerMemberBehavior.Disallow;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		Type = TargetType.Game;

		ExtraModuleNames.Add("Uplift");
        
        ProjectDefinitions.Add("STARFIRE_BUILD_RTM=0");
	}
}
