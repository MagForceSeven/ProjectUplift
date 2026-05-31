// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"

#include "DataStoreActors/Hero.h"

#include "CampaignPIESettings.generated.h"

class UCampaignDifficultyDefinition;

// Settings for the PIE Configuration
UCLASS( Config = "Game" )
class CAMPAIGN_API UCampaignPIESettings : public UDeveloperSettings
{
	GENERATED_BODY( )
public:
	// The difficulty to apply when starting a PIE session
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Config, Category = "General", meta = (DisplayThumbnail = false) )
	TSoftObjectPtr< const UCampaignDifficultyDefinition > Difficulty;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Config, meta = (InlineEditConditionToggle) )
	bool bForceSeed = false;
	
	// Seed to use in PIE Sessions
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Config, Category = "General", meta = (EditCondition = "bForceSeed") )
	int32 RandomSeed = 0;

	// The heroes that should make up the player squad when launching tactical PIE
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Tactical" )
	TArray< FHeroSpec > PlayerSquad;

	// Additional heroes to add to the player roster when launching PIE
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Strategy" )
	TArray< FHeroSpec > AdditionalRoster;
	
	// Developer Settings API
	FName GetContainerName( ) const override;
	FName GetCategoryName( ) const override;
	FName GetSectionName( ) const override;

#if WITH_EDITOR
	FText GetSectionText( ) const override;
	FText GetSectionDescription( ) const override;
#endif
};