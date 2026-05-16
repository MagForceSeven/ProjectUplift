// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"

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

	// Developer Settings API
	FName GetContainerName( ) const override;
	FName GetCategoryName( ) const override;
	FName GetSectionName( ) const override;

#if WITH_EDITOR
	FText GetSectionText( ) const override;
	FText GetSectionDescription( ) const override;
#endif
};