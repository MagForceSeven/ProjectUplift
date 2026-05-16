// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"

#include "CampaignSettings.generated.h"

// General settings for the normal campaign gameplay
UCLASS( Config = "Game", DefaultConfig )
class CAMPAIGN_API UCampaignSettings : public UDeveloperSettings
{
	GENERATED_BODY( )
public:
	// The level used for the main menu
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Config )
	TSoftObjectPtr< const UWorld > ShellLevel;

	// The level used for the strategy layer
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Config )
	TSoftObjectPtr< const UWorld > StrategyLevel;

	// Developer Settings API
	FName GetContainerName( ) const override;
	FName GetCategoryName( ) const override;
	FName GetSectionName( ) const override;

#if WITH_EDITOR
	FText GetSectionText( ) const override;
	FText GetSectionDescription( ) const override;
#endif
};