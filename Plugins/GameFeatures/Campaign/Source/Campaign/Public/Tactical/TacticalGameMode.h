// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/CampaignGameMode.h"

#include "NativeGameplayTags_SF.h"

#include "TacticalGameMode.generated.h"

// Tactical hook into the game mode chains
UCLASS( )
class CAMPAIGN_API ATacticalGameMode : public ACampaignGameMode
{
	GENERATED_BODY( )
public:
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( WorldType_Tactical );

	ATacticalGameMode( );

protected:
	// Campaign Game Mode API
	void InitializeCommon( void ) override;
	void InitializeForQuickPlay( void ) override;
	void TransitionIntoMode( void ) override;
	void GameModeReady( void ) override;
	void PreTransitionOutOfMode( void ) override;
	TArray< FPrimaryAssetId > GatherAssetsForModeBundles( void ) const override;
};