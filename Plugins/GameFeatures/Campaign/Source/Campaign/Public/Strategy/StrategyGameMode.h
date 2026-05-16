// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/CampaignGameMode.h"

#include "NativeGameplayTags_SF.h"

#include "StrategyGameMode.generated.h"

// Strategy hook into the game mode chains
UCLASS( )
class CAMPAIGN_API AStrategyGameMode : public ACampaignGameMode
{
	GENERATED_BODY( )
public:
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( WorldType_Strategy )

	AStrategyGameMode( );

	// Start a New Game
	UFUNCTION( BlueprintCallable, meta = (WorldContext = "WorldContext") )
	static void LaunchNewGame( const UObject *WorldContext );

protected:
	// Campaign Game Mode API
	void InitializeCommon( void ) override;
	void InitializeForQuickPlay( void ) override;
	void TransitionIntoMode( void ) override;
	void GameModeReady( void ) override;
	void PreTransitionOutOfMode( void ) override;
};