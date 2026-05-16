// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "StarfireGameUserSettings.h"

#include "UpliftGameUserSettings.generated.h"

// The collection of per user configuration options relevant to the game
UCLASS( Config = GameUserSettings )
class UPLIFT_API UUpliftGameUserSettings : public UStarfireGameUserSettings
{
	GENERATED_BODY( )
public:
	// Settings accessor
	static UUpliftGameUserSettings* Get( );

private:
	// Get the CoreTech plugin settings for the current user
	UFUNCTION( BlueprintCallable, BlueprintPure = true, Category = "Game Globals", meta = (DisplayName = "Get Uplift Game User Settings") )
	static UUpliftGameUserSettings* GetUplift( ) { return Get( ); }
};