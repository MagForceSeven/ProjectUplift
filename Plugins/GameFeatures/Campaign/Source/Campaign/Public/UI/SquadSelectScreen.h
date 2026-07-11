// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "StarfireScreen.h"

#include "SquadSelectScreen.generated.h"

class UMissionDeploymentVM;

// Native base for a screen intended to configure and launch a tactical session
UCLASS( Abstract )
class CAMPAIGN_API USquadSelectScreen : public UStarfireScreen
{
	GENERATED_BODY( )
public:

protected:	
	// Begin the transition into tactical using the choices stored in the VM
	UFUNCTION( BlueprintCallable, meta = (HidePinAssetPicker = "VM") )
	void TransitionToTactical( const UMissionDeploymentVM *VM );
};