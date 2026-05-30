// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "DataStoreSingleton.h"
#include "Framework/GameModeAssetProvider.h"
#include "Templates/ActorSingletonAccessor.h"

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

// The configuration of the battle
UCLASS( Blueprintable )
class CAMPAIGN_API ADS_BattleData : public ADataStoreSingleton, public IGameModeAssetProvider, public TActorSingletonAccessors< ADS_BattleData >
{
	GENERATED_BODY( )
public:
	// How this battle was started
	UPROPERTY( BlueprintReadOnly )
	ETacticalMode TacticalMode = ETacticalMode::Campaign;

	// Game Mode Asset Provider
	TSet< FPrimaryAssetId > GatherAssetsForModeBundles_Implementation( const FGameplayTag &Mode ) const override;
};