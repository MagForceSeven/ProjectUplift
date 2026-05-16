// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Tactical/TacticalPlayerModeBase.h"

#include "PlayerMode_TacticalIdle.generated.h"

// Player mode for the idle state of tactical gameplay (waiting for player input)
UCLASS( )
class CAMPAIGN_API APlayerMode_TacticalIdle : public ATacticalPlayerModeBase
{
	GENERATED_BODY( )
public:
	APlayerMode_TacticalIdle( );
};