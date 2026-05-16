// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "PlayerModeBase.h"

#include "TacticalPlayerModeBase.generated.h"

// Base class for all tactical related player modes
UCLASS( Abstract )
class CAMPAIGN_API ATacticalPlayerModeBase : public APlayerModeBase
{
	GENERATED_BODY( )
public:
	ATacticalPlayerModeBase( );
};