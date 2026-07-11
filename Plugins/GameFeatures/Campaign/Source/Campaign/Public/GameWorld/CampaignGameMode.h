// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/UpliftGameMode.h"

#include "NativeGameplayTags_SF.h"

#include "CampaignGameMode.generated.h"

struct FStreamableHandle;
class UDataDefinition;

// Series of initialization steps before the game mode is ready to start the match
enum class ECampaignInitializationStage
{
	Init,
	WaitOnFeatures,
	InitGameData,
	ToggleBundles,
	WaitOnBundles,
	Complete
};

// Core plugin hook into the game mode chains
UCLASS( )
class CAMPAIGN_API ACampaignGameMode : public AUpliftGameMode
{
	GENERATED_BODY( )
public:
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( WorldType_Gameplay );

	ACampaignGameMode( );

	// Game Mode API
	void HandleMatchIsWaitingToStart( ) override;
	bool ReadyToStartMatch_Implementation( ) override;

	// Actor API
	void BeginPlay( ) override;
	void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

	// Utility for returning to the shell from campaign gameplay
	UFUNCTION( BlueprintCallable, meta = (WorldContext = "WorldContext" ) )
	static void ReturnToShell( const UObject *WorldContext );

	// Utility function for doing all the tasks needed for transitioning from one gameplay level to another
	static void TransitionToNewGameplayLevel( const UObject *WorldContext, const TSoftObjectPtr< const UWorld > &NewLevel );

#if !UE_BUILD_SHIPPING
	// Functions for use by cheats or other developer tools which want to bring in the bundle assets for the current mode
	// The bundles loads will immediately block to wait for the bundle assets (if any) to become loaded
	// Those assets will remain loaded for the remainder of the mode
	void DEBUG_AddGameModeAsset( const UDataDefinition *Definition );
	void DEBUG_AddGameModeAsset( const FPrimaryAssetId &AssetID );
#endif

protected:
	// The bundles that are enabled on owned feature content assets to extend the Main Menu options
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
	TArray< FName > GameModeBundles;

	// Hook for initializing anything not dependent on game data or actors that wouldn't be restored by a save game. Pre-BeginPlay
	virtual void InitializeCommon( void );

	// Hook for filling out the game data when launching in PIE or another tool that may not be a "real" game execution. Pre-BeginPlay
	virtual void InitializeForQuickPlay( void );

	// Hook for a real game transition from one mode to another. Called for travel saves only. Pre-BeginPlay
	virtual void TransitionIntoMode( void );

	// Hook that the game mode and world actors have all begun play and the mode should be ready to start gameplay. Post-BeginPlay
	virtual void GameModeReady( void );

	// Hook for changes that should be made immediately prior to the creation of a travel save.
	virtual void PreTransitionOutOfMode( void );

	// Hook for the derived game modes to specify which assets should have the game mode asset bundles loaded for them
	virtual TSet< FPrimaryAssetId > GatherAssetsForModeBundles( void ) const;

private:
    // Internal hook for coordinating the population of game data, either from a save or from quickplay configuration settings
    void InitGameModelData( void );

	// Common campaign initialization for a new game being started regardless of how it was started or the scope of gameplay
	void InitializeForNewGame( void );

	// The current initialization step of the mode
	ECampaignInitializationStage InitStage = ECampaignInitializationStage::Init;

	// The handle to async asset streaming requests
	TSharedPtr< FStreamableHandle > StreamHandle;

	// The collection of assets that had bundles applied so that they can be undone when leaving the mode
	TArray< FPrimaryAssetId > GameModeBundleAssets;
};
