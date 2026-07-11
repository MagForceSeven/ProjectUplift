// Copyright Russell Aasland. All Rights Reserved.

using UnrealBuildTool;

public class Campaign : ModuleRules
{
	public Campaign(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"DeveloperSettings",
				"Engine",
				"EnhancedInput",
				"GameFeatures",
				"GameplayTags",
				"ModelViewViewModel",
				"SlateCore",
				"UMG",

				"ModularGameplayActors",
				"CommonGame",
				"CommonUI",

				"ActorCollections",
				"ActorViewModels",
				"GameFacts",
				"GameFeatureSubsystems",
				"LevelMetadata",
				"PlayerModes",
				"StarfireAssets",
				"StarfireDataActors",
				"StarfireGameCore",
				"StarfireMessenger",
				"StarfirePersistence",
				"StarfireSaveData",
				"StarfireUI",
				"StarfireUtilities",

				"Uplift",
				// ... add other public dependencies that you statically link with here ...
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add( "UnrealEd" );
		}
	}
}
