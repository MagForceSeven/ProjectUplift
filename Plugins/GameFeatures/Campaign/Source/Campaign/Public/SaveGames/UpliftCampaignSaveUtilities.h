// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "SaveData/SaveDataUtilities.h"

#include "UpliftCampaignSaveUtilities.generated.h"

class UUpliftCampaignSave;
class UUpliftCampaignSaveHeader;

// A categorization of save games
UENUM( BlueprintType )
enum class ESaveGameType : uint8
{
	// Created at the direction of the player
	User,

	// Created by player, but auto named
	Quick,

	// Created at the direction of the game
	Auto,

	// Same as Auto, but at a time that won't ship
	Developer,
};

// Condensed information for a save that has been found on disk
USTRUCT( BlueprintType )
struct CAMPAIGN_API FEnumeratedSaveGameHeader
{
	GENERATED_BODY( )
public:
	FEnumeratedSaveGameHeader( ) = default;
	explicit FEnumeratedSaveGameHeader( const USaveDataUtilities::FEnumeratedHeader_Core &Core );
	explicit FEnumeratedSaveGameHeader( ESaveDataLoadResult R ) : LoadingResult( R ) { }

	// The name of the save slot associated with the save
	UPROPERTY( BlueprintReadOnly, Category = "Header" )
	FString SlotName;

	// The results from loading this particular save (or at least from loading the header)
	UPROPERTY( BlueprintReadOnly, Category = "Header" )
	ESaveDataLoadResult LoadingResult = ESaveDataLoadResult::SerializationFailed;

	// The header information for this particular save
	UPROPERTY( BlueprintReadOnly, Category = "Header" )
	TObjectPtr< const UUpliftCampaignSaveHeader > Header = nullptr;
};

// Callback for the completion of an async save process
DECLARE_DELEGATE_ThreeParams( FSaveAsyncCallback, const FString& /*SlotName*/, int32 /*UserIndex*/, bool /*Success*/ );
// Callback for the completion of an async load process
DECLARE_DELEGATE_FiveParams( FLoadAsyncCallback, const FString& /*SlotName*/, int32 /*UserIndex*/, ESaveDataLoadResult /*Result*/, const UUpliftCampaignSaveHeader* /*Header*/, const UUpliftCampaignSave* /*SaveData*/ );
// Callback for the completion of an async load process for just a save game header
DECLARE_DELEGATE_FourParams( FLoadHeaderAsyncCallback, const FString& /*SlotName*/, int32 /*UserIndex*/, ESaveDataLoadResult /*Result*/, const UUpliftCampaignSaveHeader* /*Header*/ );
// Delegate that can be used to filter a collection of saves
DECLARE_DELEGATE_RetVal_FourParams( bool, FSaveFilter, const FString& /*SlotName*/, int32 /*UserIndex*/, const UUpliftCampaignSaveHeader* /*Header*/, ESaveDataLoadResult /*LoadingResult*/ );
// Callback for the completion of an async check for the existence of save games
DECLARE_DELEGATE_OneParam( FSavesExistAsyncCallback, bool /*Success*/ );
// Callback for the completion of an async enumeration of all the save headers
DECLARE_DELEGATE_OneParam( FEnumerateHeadersComplete, const TArray< FEnumeratedSaveGameHeader > &Headers );
// Callback for the completion of an async creation of save game data (from the time that the checkpoint was requested, not from the callback invocation)
DECLARE_DELEGATE_ThreeParams( FCreateCheckpointComplete, const UObject* /*WorldContext*/, const UUpliftCampaignSave* /*CheckpointData*/, bool /*Success*/ );

// Game specific utilities for saving the state of the game to a file
UCLASS( )
class CAMPAIGN_API UUpliftCampaignSaveUtilities : public USaveDataUtilities
{
	GENERATED_BODY( )
public:
	// Determine the list of filenames currently in use
	UFUNCTION( BlueprintCallable, Category = "Save Games" )
	[[nodiscard]] static TArray< FString > EnumerateSlotNames( int32 UserIndex );

	// Determine a slot name that would work for a particular type of save
	UFUNCTION( BlueprintCallable, Category = "Save Games", meta = (AdvancedDisplay = "SaveType") )
	[[nodiscard]] static FString GetUnusedSlotName( int32 UserIndex, ESaveGameType SaveType );

	// Remove a save slot from the disk
	UFUNCTION( BlueprintCallable, Category = "Save Games", meta = (WorldContext = "WorldContext") )
	[[nodiscard]] static bool DeleteSaveGameInSlot( const UObject *WorldContext, const FString &SlotName, int32 UserIndex );
	
	// Determine if a specific save slot name is in use
	UFUNCTION( BlueprintCallable, Category = "Save Games" )
	[[nodiscard]] static bool DoesSaveGameExist( const FString &SlotName, int32 UserIndex );

	// Check if the game currently allows manual saves to be created
	UFUNCTION( BlueprintCallable, BlueprintPure = true, Category = "Save Games", meta = (WorldContext = "WorldContext") )
	[[nodiscard]] static bool IsManualSavingAllowed( const UObject *WorldContext );

	// Explicit call to load headers for the available saves
	UFUNCTION( BlueprintCallable, Category = "Save Games", meta = (WorldContext = "WorldContext") )
	static void CacheSaveGameHeaders( const UObject *WorldContext, int UserIndex );

	// Save the current state of the game to a file
	[[nodiscard]] static bool SaveToSlot( const UObject *WorldContext, FString SlotName, int32 UserIndex, ESaveGameType SaveType, FString DisplayNameOverride = { } );
	// Save the current state of the game to a file asynchronously
	static void SaveToSlot_Async( const UObject *WorldContext, FString SlotName, int32 UserIndex, ESaveGameType SaveType, FString DisplayNameOverride = { }, const FSaveAsyncCallback &OnCompletion = FSaveAsyncCallback( ) );

	// Save a pre-existing checkpoint to a file
	static void SaveCheckpointToSlot( const UObject *WorldContext,const UUpliftCampaignSave *CheckpointData, FString SlotName, int32 UserIndex, ESaveGameType SaveType, FString DisplayNameOverride = { } );
	// save a pre-existing checkpoint to a file asynchronously
	static void SaveCheckpointToSlot_Async( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, FString SlotName, int32 UserIndex, ESaveGameType SaveType, FString DisplayNameOverride = { }, const FSaveAsyncCallback &OnCompletion = FSaveAsyncCallback( ) );

	// Load an in-memory save as if it were a save from a slot
	static void LoadCheckpointSave( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData );
	
	// Try to load the data from a slot into the save game data structure
	[[nodiscard]] static ESaveDataLoadResult LoadSaveGameFromSlot( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, const UUpliftCampaignSaveHeader *& outHeader, const UUpliftCampaignSave *& outSaveData );
	// Try to load the data from a slot asynchronously and report back when complete with the save data
	static void LoadSaveGameFromSlot_Async( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, const FLoadAsyncCallback &OnCompletion );

	// Find the most recent save (that meets an optional filter requirements) and load it 
	[[nodiscard]] static bool LoadMostRecentSave( const UObject *WorldContext, int32 UserIndex, FString &outSlotName, const UUpliftCampaignSaveHeader *& outHeader, const UUpliftCampaignSave *& outSaveData, const FSaveFilter &Filter = FSaveFilter( ) );
	// Find the most recent save (that meets an optional filter requirements) and load it asynchronously and report back when complete
	static void LoadMostRecentSave_Async( const UObject *WorldContext, int32 UserIndex, const FLoadAsyncCallback &OnCompletion, const FSaveFilter &Filter = FSaveFilter( ) );

	// Load just the header information for a slot
	[[nodiscard]] static ESaveDataLoadResult LoadSlotHeaderOnly( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, const UUpliftCampaignSaveHeader *& outHeader );
	// Load just the header information for a slot, calling the callback when process asynchronously completes
	static void LoadSlotHeaderOnly_Async( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, const FLoadHeaderAsyncCallback &OnCompletion );

	// Check if any saves exist on disk (that meet an optional filter requirement)
	[[nodiscard]] static bool AnySavesExist( const UObject *WorldContext, int32 UserIndex, const FSaveFilter &Filter = FSaveFilter( ) );
	// Check if any saves exist on disk (that meet an optional filter requirement), calling the callback
	static void AnySavesExist_Async( const UObject *WorldContext, int32 UserIndex, const FSavesExistAsyncCallback &OnCompletion, const FSaveFilter &Filter = FSaveFilter( ) );

	// Get the headers for all the saves that exist on disk (that meet an optional filter requirement)
	[[nodiscard]] static TArray< FEnumeratedSaveGameHeader > EnumerateSaveHeaders( const UObject *WorldContext, int32 UserIndex, const FSaveFilter &Filter = FSaveFilter( ) );
	// Get the headers for all the saves that exist on disk (that meet an optional filter requirement), calling the callback when process asynchronously completes
	static void EnumerateSaveHeaders_Async( const UObject *WorldContext, int32 UserIndex, const FEnumerateHeadersComplete &OnCompletion, const FSaveFilter &Filter = FSaveFilter( ), const FLoadHeaderAsyncCallback &OnSingleHeader = { } );

	// Get the slot/header for the most recent save (that meets on optional filter requirement)
	[[nodiscard]] static FEnumeratedSaveGameHeader FindMostRecentSave( const UObject *WorldContext, int32 UserIndex, const FSaveFilter &Filter = FSaveFilter( ) );
	// Get the slot/header for the most recent save (that meets on optional filter requirement), calling the callback when process asynchronously completes
	static void FindMostRecentSave_Async( const UObject *WorldContext, int32 UserIndex, const FLoadHeaderAsyncCallback &OnCompletion, const FSaveFilter &Filter = FSaveFilter( ) );

	// Get the slot/header for the least recent save (that meets on optional filter requirement)
	[[nodiscard]] static FEnumeratedSaveGameHeader FindLeastRecentSave( const UObject *WorldContext, int32 UserIndex, const FSaveFilter &Filter = FSaveFilter( ) );
	// Get the slot/header for the least recent save (that meets on optional filter requirement), calling the callback when process asynchronously completes
	static void FindLeastRecentSave_Async( const UObject *WorldContext, int32 UserIndex, const FLoadHeaderAsyncCallback &OnCompletion, const FSaveFilter &Filter = FSaveFilter( ) );

	// Create an in memory save that represents the state of the game that should be transferred to the next gameplay level
	[[nodiscard]] static const UUpliftCampaignSave* CreateTravelSave( const UObject *WorldContext, const TSoftObjectPtr< const UWorld > &Destination );
	
	// Create an in memory save that represents the state of the game at this moment
	[[nodiscard]] static const UUpliftCampaignSave* CreateCheckpointSave( const UObject *WorldContext );
	// Create an in memory save that will be filled in asynchronously but represents the state of the game from the moment it was called 
	static void CreateCheckpointSave_Async( const UObject *WorldContext, const FCreateCheckpointComplete &OnCompletion );

	// Create and write an automatic save to a file - either to an open numbered slot or to the slot that's been around longest
	static bool AutoSave( const UObject *WorldContext, int32 UserIndex, const FString &DisplayNameOverride = { } );
	// Create and write an automatic save to a file asynchronously - either to an open numbered slot or to the slot that's been around longest
	static void AutoSave_Async( const UObject *WorldContext, int32 UserIndex, const FString &DisplayNameOverride = { }, const FSaveAsyncCallback &OnCompletion = FSaveAsyncCallback( ) );

	// Create and write a save to a file - always to slot 'QuickSave' - always user triggered
	static bool QuickSave( const UObject *WorldContext, int32 UserIndex );
	// Create and write a save to a file asynchronously - always to slot 'QuickSave' - always user triggered
	static void QuickSave_Async( const UObject *WorldContext, int32 UserIndex, const FSaveAsyncCallback &OnCompletion = FSaveAsyncCallback( ) );
	
	// Save the current state of the game to a file
	// Files starting with "/" will be assumed to be coming from ProjectContent, otherwise specify a fully qualified path
	[[nodiscard]] static bool SaveToPath( const UObject *WorldContext, const FString &PathName, ESaveGameType SaveType, FString DisplayNameOverride = { } );
	// Save the current state of the game to a file asynchronously
	// Files starting with "/" will be assumed to be coming from ProjectContent, otherwise specify a fully qualified path
	static void SaveToPath_Async( const UObject *WorldContext, const FString &PathName, ESaveGameType SaveType, FString DisplayNameOverride = { }, const FSaveAsyncCallback &OnCompletion = FSaveAsyncCallback( ) );

	// Save a pre-existing checkpoint to a file
	// Files starting with "/" will be assumed to be coming from ProjectContent, otherwise specify a fully qualified path
	static void SaveCheckpointToPath( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, const FString &PathName, ESaveGameType SaveType, FString DisplayNameOverride = { } );
	// save a pre-existing checkpoint to a file asynchronously
	// Files starting with "/" will be assumed to be coming from ProjectContent, otherwise specify a fully qualified path
	static void SaveCheckpointToPath_Async( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, const FString &PathName, ESaveGameType SaveType, FString DisplayNameOverride = { }, const FSaveAsyncCallback &OnCompletion = FSaveAsyncCallback( ) );

	// Try to load the data from a slot into the save game data structure
	// Files starting with "/" will be assumed to be coming from ProjectContent, otherwise specify a fully qualified path
	[[nodiscard]] static ESaveDataLoadResult LoadSaveGameFromPath( const UObject *WorldContext, const FString &PathName, const UUpliftCampaignSaveHeader *& outHeader, const UUpliftCampaignSave *& outSaveData );
	// Try to load the data from a slot asynchronously and report back when complete with the save data
	// Files starting with "/" will be assumed to be coming from ProjectContent, otherwise specify a fully qualified path
	static void LoadSaveGameFromPath_Async( const UObject *WorldContext, const FString &PathName, const FLoadAsyncCallback &OnCompletion );

	// Load just the header information for a slot
	// Files starting with "/" will be assumed to be coming from ProjectContent, otherwise specify a fully qualified path
	[[nodiscard]] static ESaveDataLoadResult LoadPathHeaderOnly( const UObject *WorldContext, const FString &PathName, const UUpliftCampaignSaveHeader *& outHeader );
	// Load just the header information for a slot, calling the callback when process asynchronously completes
	// Files starting with "/" will be assumed to be coming from ProjectContent, otherwise specify a fully qualified path
	static void LoadPathHeaderOnly_Async( const UObject *WorldContext, const FString &PathName, const FLoadHeaderAsyncCallback &OnCompletion );

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	// A development only automatic save created with the current state of the game and written to disk
	static bool DeveloperSave( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, FString DisplayNameOverride = { } );
	// A development only automatic save created with the current state of the game and written to disk asynchronously
	static void DeveloperSave_Async( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, FString DisplayNameOverride = { }, const FSaveAsyncCallback &OnCompletion = FSaveAsyncCallback( ) );
#endif

protected:
	// Determine the best name for an auto save, either an unused slot name or the oldest save with the AutoSave type
	[[nodiscard]] static FString FindBestAutoSaveSlotName( const UObject *WorldContext, int32 UserIndex );
	// Determine the best name for an auto save asynchronously, either an unused slot name or the oldest save with the AutoSave type
	static void FindBestAutoSaveSlotName_Async( const UObject *WorldContext, int32 UserIndex, const FSaveAsyncCallback &OnCompletion );
	
	// Create the save game object and fill all the data requiring immediate synchronous data
	[[nodiscard]] static UUpliftCampaignSave* CreateSaveData( const UObject *WorldContext, bool bIsTravelSave = false );
	// Fill the save with the data that can be populated asynchronously
	[[nodiscard]] static bool FillAsyncSaveGameData( UUpliftCampaignSave *SaveGame );
	// Fill a save with save data asynchronously
	static void FillAsyncSaveGameData_Async( const UObject *WorldContext, UUpliftCampaignSave *SaveGame, bool bIncludeCheckpoints, const FCreateCheckpointComplete &OnCompletion );
	// Fill a checkpoint with data that would not have been available at the time that it was created but should be included when saved to disk
	[[nodiscard]] static bool FillCheckpointData( const UObject *WorldContext, UUpliftCampaignSave *SaveGame );
	// Fill a checkpoint with data that would not have been available at the time that it was created but should be included when saved to disk
	static void FillCheckpointData_Async( const UObject *WorldContext, UUpliftCampaignSave *SaveGame, const FCreateCheckpointComplete &OnCompletion );
	// Create the data that should be saved for the current state of the game
	[[nodiscard]] static UUpliftCampaignSave* CreateAndFillSaveData( const UObject *WorldContext, bool IncludeCheckpointData, bool bTravelSave );

	// Create the header data for the save that is meta-data about the game (mostly for UI purposes)
	[[nodiscard]] static UUpliftCampaignSaveHeader* CreateSaveGameHeader( const UUpliftCampaignSave *SaveGame, ESaveGameType SaveType, const FString &DisplayName );
};