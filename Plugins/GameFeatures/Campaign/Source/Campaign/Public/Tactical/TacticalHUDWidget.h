// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/CampaignHUDWidget.h"

#include "TacticalHUDWidget.generated.h"

// Tactical implementation of a widget to act as the primary HUD widget
UCLASS( )
class CAMPAIGN_API UTacticalHUDWidget : public UCampaignHUDWidget
{
	GENERATED_BODY( )
public:

protected:
	// Transition back to the strategy map
	UFUNCTION( BlueprintCallable )
	void TransitionToStrategy( );
};