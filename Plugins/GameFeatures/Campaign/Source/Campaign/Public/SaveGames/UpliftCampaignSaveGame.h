// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "SaveData/SaveData.h"

#include "GameplayTagContainer.h"

#include "UpliftCampaignSaveGame.generated.h"

enum class ESaveGameType : uint8;
struct FStreamableHandle;

// Game specific save game information
UCLASS( )
class CAMPAIGN_API UUpliftCampaignSave : public USaveData
{
	GENERATED_BODY( )
public:
	// Core Save Header API
	[[nodiscard]] bool IsCompatible( uint32 InVersion, uint32 InChangelist ) const override;

	// Apply this save game to the game data
	bool ApplySaveData( const UObject *WorldContext ) const;

	// The type of save game this is
	UPROPERTY( VisibleInstanceOnly )
	ESaveGameType SaveType;

	// The world that should be loaded into
	// This is either where we were to make the save, or set by the game setup screen
	UPROPERTY( VisibleInstanceOnly )
	TSoftObjectPtr< const UWorld > WorldToLoad;

	// The type of world the player was in when they made the save
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	FGameplayTag WorldType;

	// The user facing display name
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	FText UserDisplayName;

	// The user facing display name was auto-generated
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	bool bAutomatedDisplayName = false;

	// Mode specific descriptor for UI display
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Data" )
	FText Descriptor1;

	// Mode specific descriptor for UI display
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Data" )
	FText Descriptor2;

	// An ID that can correlate saves to a specific playthrough
	UPROPERTY( VisibleInstanceOnly )
	FGuid CampaignID;

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