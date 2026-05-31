// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "NativeGameplayTags_SF.h"

#include "GameplayTagContainer.h"

#include "NewGameSettings.generated.h"

class UStarfireFeatureData;
class UCampaignDifficultyDefinition;

// Configuration options for a new game started from the Shell
UCLASS( BlueprintType )
class UNewGameSettings : public UObject
{
	GENERATED_BODY( )
public:
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( NewGameFlag_Root );
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( NewGameFlag_TutorialsEnabled );
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( NewGameFlag_FirstTimeUserExp );
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( NewGameFlag_IsCampaign );

	// Flags to apply to the setup of this campaign
	UPROPERTY( EditInstanceOnly, BlueprintReadWrite, meta = (Categories = "NewGame.Flags"))
	FGameplayTagContainer CampaignFlags;

	// Content Packages to enable at the start of the campaign
	UPROPERTY( EditInstanceOnly, BlueprintReadWrite )
	TArray< TObjectPtr< const UStarfireFeatureData > > Packages;

	// The selected difficulty for the new game
	UPROPERTY( EditInstanceOnly, BlueprintReadWrite )
	TObjectPtr< UCampaignDifficultyDefinition > Difficulty;

	// Seed for this new campaign
	UPROPERTY( EditInstanceOnly, BlueprintReadWrite )
	int32 RandomSeed = 0;
};