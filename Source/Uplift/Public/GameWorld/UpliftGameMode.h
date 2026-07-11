// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "StarfireGameGameMode.h"

#include "UpliftGameMode.generated.h"

class UGameModeViewModels;

// Project hook into the game mode chains
UCLASS( )
class UPLIFT_API AUpliftGameMode : public AStarfireGameGameMode
{
	GENERATED_BODY( )
public:
	AUpliftGameMode( );

protected:
	// Source of singleton view models that should be created at a minimum for this mode
	UPROPERTY( VisibleDefaultsOnly )
	TObjectPtr< UGameModeViewModels > DefaultViewModels;
};