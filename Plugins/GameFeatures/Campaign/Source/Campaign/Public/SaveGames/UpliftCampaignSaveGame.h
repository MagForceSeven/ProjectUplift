// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "SaveData/SaveData.h"

#include "UpliftCampaignSaveGame.generated.h"

struct FStreamableHandle;

// Game specific save game information
UCLASS( )
class CAMPAIGN_API UUpliftCampaignSave : public USaveData
{
	GENERATED_BODY( )
public:
	// Core Save Header API
	[[nodiscard]] bool IsCompatible( uint32 InVersion ) const override;

	// Apply this save game to the game data
	bool ApplySaveData( const UObject *WorldContext ) const;

	// Is this a save we made before traveling into a new level? If this is false,
	// we assume this save was made in the middle of gameplay.
	UPROPERTY( VisibleInstanceOnly )
	bool bTravelSave = false;

	// The world that should be loaded into
	// This is either where we were to make the save, or set by the game setup screen
	UPROPERTY( VisibleInstanceOnly )
	TSoftObjectPtr< const UWorld > WorldToLoad;

	// Mode specific descriptor for UI display
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Data" )
	FText Descriptor1;

	// Mode specific descriptor for UI display
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Data" )
	FText Descriptor2;

	// Binary data for persistent actors
	UPROPERTY( VisibleInstanceOnly )
	TArray< uint8 > PersistentActorData;

	// The classes that should get loaded to reconstruct the actors in PersistentActorData
	UPROPERTY( VisibleInstanceOnly )
	TArray< FSoftObjectPath > PersistentActorClasses;

	// A serialized version of the tactical restart checkpoint
	UPROPERTY( VisibleInstanceOnly )
	TArray< uint8 > TacticalCheckpointBytes;

	// Un-serialized flag tracking the (potentially async) completion
	bool bCreationComplete = false;
	
	// Start loading the assets required for the contents of this save
	TSharedPtr< FStreamableHandle > LoadAssets( void ) const;
};