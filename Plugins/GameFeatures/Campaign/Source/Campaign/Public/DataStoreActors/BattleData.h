// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "DataStoreSingleton.h"

#include "BattleData.generated.h"

// Enumeration of the ways that a tactical map could be started
UENUM( )
enum class ETacticalMode : uint8
{
	// Regular gameplay
	Campaign,
	// Tactical match started from the Main Menu
	Simulator,
	// Tactical match started as a PlayInEditor tactical map
	PIE,
};

//
UCLASS( Blueprintable )
class CAMPAIGN_API ADS_BattleData : public ADataStoreSingleton
{
	GENERATED_BODY( )
public:
	//
	UPROPERTY( BlueprintReadOnly )
	ETacticalMode TacticalMode = ETacticalMode::Campaign;
};