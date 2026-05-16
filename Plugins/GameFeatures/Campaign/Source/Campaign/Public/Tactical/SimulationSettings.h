// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "GameplayTagContainer.h"

#include "SimulationSettings.generated.h"

class UStarfireFeatureData;
class UCampaignDifficultyDefinition;

// Data that can be filled out to start a tactical simulation from the Shell
UCLASS( BlueprintType )
class USimulationSettings : public UObject
{
	GENERATED_BODY( )
public:
	// Flags to apply to the setup of this campaign
	UPROPERTY( EditInstanceOnly, BlueprintReadWrite, meta = (Categories = "NewGame.Flags"))
	FGameplayTagContainer CampaignFlags;

	// Content Packages to enable at the start of the campaign
	UPROPERTY( EditInstanceOnly, BlueprintReadWrite )
	TArray< TObjectPtr< const UStarfireFeatureData > > Packages;
	
	// The selected difficulty for the new game
	UPROPERTY( EditInstanceOnly, BlueprintReadWrite )
	TObjectPtr< const UCampaignDifficultyDefinition > Difficulty;
};