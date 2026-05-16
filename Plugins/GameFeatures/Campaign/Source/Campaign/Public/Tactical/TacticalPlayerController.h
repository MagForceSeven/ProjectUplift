// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/CampaignPlayerController.h"

#include "TacticalPlayerController.generated.h"

class UDefaultPlayerModeComponent;

// Base type for player states to be associated with TacticalGameMode
UCLASS( )
class CAMPAIGN_API ATacticalPlayerController : public ACampaignPlayerController
{
	GENERATED_BODY( )
public:
	ATacticalPlayerController( );

private:
	// The initial mode that should be used to populate the Player Modes stack for this player
	UPROPERTY( VisibleDefaultsOnly, Category = "Player Modes" )
	TObjectPtr< UDefaultPlayerModeComponent > DefaultPlayerMode;
};