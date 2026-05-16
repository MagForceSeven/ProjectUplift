// Copyright Russell Aasland. All Rights Reserved.

#include "SaveGames/UpliftCampaignSaveBlueprintUtilities.h"

#include "SaveGames/UpliftCampaignSaveUtilities.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftCampaignSaveBlueprintUtilities)

UEnumerateSaveGameHeaders_AsyncAction* UEnumerateSaveGameHeaders_AsyncAction::EnumerateSaveGameHeaders( int UserIndex, const FSaveGameFilter &SaveFilter, UObject *WorldContext )
{
	const auto Action = NewObject< UEnumerateSaveGameHeaders_AsyncAction >( WorldContext );

	Action->UserIndex = UserIndex;
	Action->Filter = SaveFilter;

	return Action;
}

void UEnumerateSaveGameHeaders_AsyncAction::Activate( void )
{
	FSaveFilter SaveFilter;
	if (Filter.IsBound( ))
	{
		SaveFilter = FSaveFilter::CreateLambda([ this ]( const FString &SlotName, int32 ParamUserIndex, const UUpliftCampaignSaveHeader *Header, ESaveDataLoadResult LoadingResult ) -> bool
		{
			return Filter.Execute( SlotName, ParamUserIndex, Header, LoadingResult );
		});
	}

	const auto OnUtilityComplete = FEnumerateHeadersComplete::CreateLambda([ this ]( const TArray< FEnumeratedSaveGameHeader > &Headers ) -> void
	{
		OnComplete.Broadcast( UserIndex, Headers );

		EndAction( );
	});

	UUpliftCampaignSaveUtilities::EnumerateSaveHeaders_Async( this, UserIndex, OnUtilityComplete, SaveFilter );

	StartAction( this, false );
}

UForEachSaveGameHeaders_AsyncAction* UForEachSaveGameHeaders_AsyncAction::ForEachSaveHeader( int UserIndex, UObject *WorldContext )
{
	const auto Action = NewObject< UForEachSaveGameHeaders_AsyncAction >( WorldContext );

	Action->UserIndex = UserIndex;

	return Action;
}

void UForEachSaveGameHeaders_AsyncAction::Activate( void )
{
	const auto OnSingleHeader = FLoadHeaderAsyncCallback::CreateLambda( [ this ]( const FString &SlotName, int32 ParamUserIndex, ESaveDataLoadResult LoadingResult, const UUpliftCampaignSaveHeader *Header )
	{
		Loop.Broadcast( SlotName, Header, LoadingResult );
	} );
	
	const auto OnUtilityComplete = FEnumerateHeadersComplete::CreateLambda([ this ]( const TArray< FEnumeratedSaveGameHeader > &Headers ) -> void
	{
		OnComplete.Broadcast( );

		EndAction( );
	});

	UUpliftCampaignSaveUtilities::EnumerateSaveHeaders_Async( this, UserIndex, OnUtilityComplete, { }, OnSingleHeader );

	StartAction( this, false );
}

ULoadSaveGame_AsyncAction* ULoadSaveGame_AsyncAction::LoadSaveGame( const FString &SlotName, int UserIndex, UObject *WorldContext )
{
	const auto Action = NewObject< ULoadSaveGame_AsyncAction >( WorldContext );
	ensureAlways( UserIndex >= 0 );

	Action->SlotName = SlotName;
	Action->UserIndex = UserIndex;

	return Action;
}

ULoadSaveGame_AsyncAction* ULoadSaveGame_AsyncAction::LoadSaveGameFromFile( const FString &PathName, UObject *WorldContext )
{
	const auto Action = NewObject< ULoadSaveGame_AsyncAction >( WorldContext );

	Action->SlotName = PathName;
	Action->UserIndex = -1;

	return Action;
}

void ULoadSaveGame_AsyncAction::Activate( void )
{
	if (UserIndex >= 0)
		UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot_Async( this, SlotName, UserIndex, FLoadAsyncCallback::CreateUObject( this, &ULoadSaveGame_AsyncAction::AsyncLoadComplete ) );
	else
		UUpliftCampaignSaveUtilities::LoadSaveGameFromPath_Async( this, SlotName, FLoadAsyncCallback::CreateUObject( this, &ULoadSaveGame_AsyncAction::AsyncLoadComplete ) );

	StartAction( this, false );
}

void ULoadSaveGame_AsyncAction::AsyncLoadComplete( const FString &AsyncSlotName, int32 AsyncUserIndex, ESaveDataLoadResult Result, const UUpliftCampaignSaveHeader *Header, const UUpliftCampaignSave *SaveData )
{
	OnComplete.Broadcast( Result, SlotName, UserIndex );

	EndAction( );
}

USaveSaveGame_AsyncAction* USaveSaveGame_AsyncAction::SaveGameToSlot( const FString &SlotName, int UserIndex, const FString &DisplayNameOverride, UObject *WorldContext )
{
	return SaveGameToSlot_Internal( SlotName, UserIndex, ESaveGameType::User, DisplayNameOverride, WorldContext );
}

USaveSaveGame_AsyncAction* USaveSaveGame_AsyncAction::SaveAutoSave( int UserIndex, const FString &DisplayNameOverride, UObject *WorldContext )
{
	return SaveGameToSlot_Internal( FString( ), UserIndex, ESaveGameType::Auto, DisplayNameOverride, WorldContext );
}

extern FString GetQuickSaveSlotName( void );
extern FString GetQuickSaveDisplayName( void );
USaveSaveGame_AsyncAction* USaveSaveGame_AsyncAction::SaveQuickSave( int UserIndex, UObject *WorldContext )
{
	return SaveGameToSlot_Internal( GetQuickSaveSlotName( ), UserIndex, ESaveGameType::Quick, GetQuickSaveDisplayName( ), WorldContext );
}

extern const FString DevSavePrefix;
USaveSaveGame_AsyncAction* USaveSaveGame_AsyncAction::SaveDeveloperSave( const FString &SlotName, int UserIndex, FString DisplayNameOverride, UObject *WorldContext )
{
	if (DisplayNameOverride.IsEmpty( ))
		DisplayNameOverride = SlotName.Replace( TEXT( "_" ), TEXT( " " ) );
	
	return SaveGameToSlot_Internal( DevSavePrefix + SlotName, UserIndex, ESaveGameType::Developer, DisplayNameOverride, WorldContext );
}

USaveSaveGame_AsyncAction* USaveSaveGame_AsyncAction::SaveCheckpointToSlot( const UUpliftCampaignSave *const& Checkpoint, const FString &SlotName, int UserIndex, ESaveGameType SaveType, FString DisplayNameOverride, UObject *WorldContext )
{
	const auto Action = SaveGameToSlot_Internal( SlotName, UserIndex, SaveType, DisplayNameOverride, WorldContext );
	Action->Checkpoint = Checkpoint;

	return Action;
}

USaveSaveGame_AsyncAction* USaveSaveGame_AsyncAction::SaveGameToFile( const FString &PathName, const FString &DisplayNameOverride, UObject *WorldContext )
{
	const auto Action = NewObject< USaveSaveGame_AsyncAction >( WorldContext );

	Action->SlotName = PathName;
	Action->UserIndex = -1;
	Action->SaveType = ESaveGameType::Developer;
	Action->DisplayNameOverride = DisplayNameOverride;

	return Action;
}

USaveSaveGame_AsyncAction* USaveSaveGame_AsyncAction::SaveGameToSlot_Internal( const FString &SlotName, int UserIndex, ESaveGameType SaveType, const FString &DisplayNameOverride, UObject *WorldContext )
{
	const auto Action = NewObject< USaveSaveGame_AsyncAction >( WorldContext );
	ensureAlways( UserIndex >= 0 );

	Action->SlotName = SlotName;
	Action->UserIndex = UserIndex;
	Action->SaveType = SaveType;
	Action->DisplayNameOverride = DisplayNameOverride;

	return Action;
}

void USaveSaveGame_AsyncAction::Activate( void )
{
	const auto CompletionDelegate = FSaveAsyncCallback::CreateUObject( this, &USaveSaveGame_AsyncAction::AsyncSaveComplete );
	
	if (UserIndex < 0)
		UUpliftCampaignSaveUtilities::SaveToPath_Async( this, SlotName, SaveType, DisplayNameOverride, CompletionDelegate );
	else if (SaveType == ESaveGameType::Auto) // specialized call for the auto-save slot to include finding slot name asynchronously
		UUpliftCampaignSaveUtilities::AutoSave_Async( this, UserIndex, DisplayNameOverride, CompletionDelegate );
	else if (Checkpoint != nullptr) // specialized call for the save with an existing save data
		UUpliftCampaignSaveUtilities::SaveCheckpointToSlot_Async( this, Checkpoint, SlotName, UserIndex, SaveType, DisplayNameOverride, CompletionDelegate );
	else // default save case where all the params can be forwarded directly to a generic save call
		UUpliftCampaignSaveUtilities::SaveToSlot_Async( this, SlotName, UserIndex, SaveType, DisplayNameOverride, CompletionDelegate );

	StartAction( this, false );
}

void USaveSaveGame_AsyncAction::AsyncSaveComplete( const FString &AsyncSlotName, int32 AsyncUserIndex, bool Success )
{
	if (Success)
		OnSuccess.Broadcast( );
	else
		OnFailure.Broadcast( );

	EndAction( );
}

UCreateCheckpointSave_AsyncAction* UCreateCheckpointSave_AsyncAction::CreateCheckpoint( UObject *WorldContext )
{
	return NewObject< UCreateCheckpointSave_AsyncAction >( WorldContext );
}

void UCreateCheckpointSave_AsyncAction::Activate( void )
{
	UUpliftCampaignSaveUtilities::CreateCheckpointSave_Async( this, FCreateCheckpointComplete::CreateUObject( this, &UCreateCheckpointSave_AsyncAction::AsyncCheckpointComplete ) );
	
	StartAction( this, false );
}

void UCreateCheckpointSave_AsyncAction::AsyncCheckpointComplete( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, bool Success )
{
	OnComplete.Broadcast( Success, CheckpointData );

	EndAction( );
}