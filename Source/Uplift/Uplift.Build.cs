// Copyright Russell Aasland. All Rights Reserved.

using UnrealBuildTool;

public class Uplift : ModuleRules
{
	public Uplift(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.AddRange(
			new string []
            {
				"Uplift/Private"
				// ... add other private include paths required here ...
            }
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"GameFeatures",
				"GameplayTags",

				"CommonGame",

				"GameFacts",
				"GameFeatureSubsystems",
				"StarfireAssets",
				"StarfireGameCore",
				"StarfireUI",
				// ... add other public dependencies that you statically link with here ...
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
				"Core",
				"CoreUObject",
				"Engine",
				"UMG",

				"CommonUI",
				"ModularGameplayActors",

				"ActorCollections",
				"ActorViewModels",
				"StarfireUtilities",
				// ... add private dependencies that you statically link with here ...	
			}
		);
			
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
