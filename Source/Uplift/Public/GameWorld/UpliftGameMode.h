// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "StarfireGameGameMode.h"

#include "UpliftGameMode.generated.h"

// Project hook into the game mode chains
UCLASS( )
class UPLIFT_API AUpliftGameMode : public AStarfireGameGameMode
{
	GENERATED_BODY( )
public:
	AUpliftGameMode( );
};