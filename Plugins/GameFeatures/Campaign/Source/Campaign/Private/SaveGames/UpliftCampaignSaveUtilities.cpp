// Copyright Russell Aasland. All Rights Reserved.

#include "SaveGames/UpliftCampaignSaveUtilities.h"

#include "SaveGames/UpliftCampaignSaveHeader.h"
#include "SaveGames/UpliftCampaignSaveGame.h"
#include "SaveGames/UpliftCampaignSaveSubsystem.h"

#include "SaveData/SaveBlockerBase.h"

#include "DataStoreActors/Campaign.h"

#include "GameFeatures/FeatureContentManager.h"

// Engine
#include "Kismet/GameplayStatics.h"

// Core
#include "UObject/GarbageCollection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftCampaignSaveUtilities)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General File Utilities

static const FString AutoSavePrefix = "AutoSave_";
extern const FString DevSavePrefix = "Dev_"; // extern'd to GameSaveGameBlueprintUtilities

static TAutoConsoleVariable< bool > CVar_AllowDeveloperSaves( TEXT( "Uplift.SaveGames.AllowDeveloperSaves" ),
	#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	true,
	#else
		false,
	#endif
	TEXT( "Whether or not developer save request should be honored" ), ECVF_Cheat );

static TAutoConsoleVariable< FString > CVar_QuickSaveSlotName( TEXT( "Uplift.SaveGames.QuickSaveSlotName" ), "QuickSave", TEXT( "The name of the slot to use for savegames" ), ECVF_Cheat );

FString GetQuickSaveSlotName( const FGuid &CampaignID ) { return CVar_QuickSaveSlotName.GetValueOnAnyThread( ) + "_" + CampaignID.ToString( ); }

void UpdateSlotName( const UObject *WorldContext, FString& SlotName, ESaveGameType SaveType, const FGuid &CampaignID )
{
	switch (SaveType)
	{
		case ESaveGameType::Auto:
			ensureAlways( !SlotName.IsEmpty( ) );
			SlotName = AutoSavePrefix + SlotName + "_" + CampaignID.ToString( );
			break;

		case ESaveGameType::Developer:
			ensureAlways( !SlotName.IsEmpty( ) );
			SlotName = DevSavePrefix + SlotName + "_" + CampaignID.ToString( );
			break;
			
		case ESaveGameType::User:
			if (SlotName.IsEmpty( ))
			{
				const auto TimeStamp = FDateTime::Now( );
				const auto World = GEngine->GetWorldFromContextObjectChecked( WorldContext );
				SlotName = World->GetName( ) + "_" + TimeStamp.ToString( );
			}

		default: // other types don't modify the slot name
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enumerated Save Game Header Structure

FEnumeratedSaveGameHeader::FEnumeratedSaveGameHeader( const USaveDataUtilities::FEnumeratedHeader_Core &Core ) :
	SlotName( Core.SlotName ),
	LoadingResult( Core.LoadingResult ),
	Header( CastChecked< UUpliftCampaignSaveHeader >( Core.Header, ECastCheckedType::NullAllowed ) )
{
	if (Header != nullptr) // if we have a valid header, use the slot name cached there
	{
		SlotName = Header->SlotName;
		ensureAlways( !SlotName.IsEmpty( ) );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Blueprint Function Library

FSaveGameCreated UUpliftCampaignSaveUtilities::OnSaveGameCreated;
FSaveGameDeleted UUpliftCampaignSaveUtilities::OnSaveGameDeleted;

TArray< FString > UUpliftCampaignSaveUtilities::EnumerateSlotNames( int32 UserIndex )
{
	return Super::EnumerateSlotNames( UserIndex );
}

void UUpliftCampaignSaveUtilities::CacheSaveGameHeaders( const UObject *WorldContext, int UserIndex )
{
	Super::CacheAllSaveGameHeaders( WorldContext, UUpliftCampaignSaveHeader::StaticClass( ), UserIndex );
}

bool UUpliftCampaignSaveUtilities::DeleteSaveGameInSlot( const UObject *WorldContext, const FString &SlotName, int32 UserIndex )
{
	OnSaveGameDeleted.Broadcast( UserIndex, SlotName );

	return Super::DeleteSaveGameInSlot( WorldContext, SlotName, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ) );
}

bool UUpliftCampaignSaveUtilities::DoesSaveGameExist( const FString &SlotName, int32 UserIndex )
{
	return Super::DoesSaveGameExist( SlotName, UserIndex );
}

bool UUpliftCampaignSaveUtilities::IsManualSavingAllowed( const UObject *WorldContext )
{
	return !Super::IsSaveTypeBlocked( WorldContext, UUpliftCampaignSave::StaticClass( ) );
}

bool UUpliftCampaignSaveUtilities::IsManualSavingAllowed( const UObject *WorldContext, TArray< FString > &OutBlockedReasons )
{
	return !Super::IsSaveTypeBlocked( WorldContext, UUpliftCampaignSave::StaticClass( ), &OutBlockedReasons );
}

FSaveBlockerHandle UUpliftCampaignSaveUtilities::AddSaveGameBlocker( const UObject *WorldContext, const TConstStructView< FSaveBlockerBase > &NewBlocker )
{
	return Super::AddSaveBlocker( WorldContext, UUpliftCampaignSave::StaticClass( ), NewBlocker );
}

FSaveBlockerHandle UUpliftCampaignSaveUtilities::AddSaveGameBlocker( const UObject *WorldContext, const TInstancedStruct< FSaveBlockerBase > &NewBlocker )
{
	if (!ensureAlways( NewBlocker.IsValid( ) ))
		return { };
	if (!ensureAlways( NewBlocker.GetScriptStruct( )->IsChildOf< FSaveBlockerBase >( ) ))
		return { };
	
	return Super::AddSaveBlocker( WorldContext, UUpliftCampaignSave::StaticClass( ), NewBlocker );
}

bool UUpliftCampaignSaveUtilities::SaveToSlot( const UObject *WorldContext, FString SlotName, int32 UserIndex, ESaveGameType SaveType, const FText &DisplayName )
{
	check( IsInGameThread( ) );

	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
		return false; // Ignore saving a developer save

	TArray< FString > BlockedReasons;
	if (IsSaveTypeBlocked( WorldContext, UUpliftCampaignSave::StaticClass( ), &BlockedReasons ))
	{
		UE_LOG( LogStarfireSaveData, Log, TEXT( "Request to save blocked:" ) );

		for (const auto &Reason : BlockedReasons)
			UE_LOG( LogStarfireSaveData, Log, TEXT( "\t\t%s" ), *Reason );

		return false;
	}
	
	const auto SaveData = CreateAndFillSaveData( WorldContext, DisplayName, true, SaveType );
	if (!ensureAlways( SaveData != nullptr ))
		return false;

	UpdateSlotName( WorldContext, SlotName, SaveType, SaveData->CampaignID );

	const auto Header = CreateSaveGameHeader( SaveData, SlotName, SaveType, SlotName );
	if (!ensureAlways( Header != nullptr ))
		return false;

	const auto Result = Super::SaveDataToSlot( WorldContext, Header, SaveData, SlotName, UserIndex );
	
	if (Result)
		OnSaveGameCreated.Broadcast( UserIndex, SlotName, Header );

	return Result;
}

void UUpliftCampaignSaveUtilities::SaveToSlot_Async( const UObject *WorldContext, FString SlotName, int32 UserIndex, ESaveGameType SaveType, const FText &DisplayName, const FSaveAsyncCallback &OnCompletion )
{
	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return; // Ignore saving a developer save
	}

	TArray< FString > BlockedReasons;
	if (IsSaveTypeBlocked( WorldContext, UUpliftCampaignSave::StaticClass( ), &BlockedReasons ))
	{
		UE_LOG( LogStarfireSaveData, Log, TEXT( "Request to save blocked:" ) );

		for (const auto &Reason : BlockedReasons)
			UE_LOG( LogStarfireSaveData, Log, TEXT( "\t\t%s" ), *Reason );

		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}

	const auto SaveData = CreateSaveData( WorldContext, DisplayName, SaveType );
	if (!ensureAlways( SaveData != nullptr ))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}

	const auto AsyncFillComplete = FCreateCheckpointComplete::CreateLambda( [ SlotName, UserIndex, SaveType, OnCompletion ]( const UObject *WorldContext, const UUpliftCampaignSave* CheckpointData, bool Success )
	{
		if (!Success)
		{
			OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
			return;
		}

		FString FinalSlotName = SlotName;
		UpdateSlotName( WorldContext, FinalSlotName, SaveType, CheckpointData->CampaignID );		

		const UUpliftCampaignSaveHeader* Header = CreateSaveGameHeader( CheckpointData, FinalSlotName, SaveType, SlotName );
		if (!ensureAlways( Header != nullptr ))
		{
			OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
			return;
		}

		const auto OnCoreCompletion = FSaveAsyncCallback_Core::CreateLambda( [ OnCompletion, Header ]( const FString &SlotName, int32 UserIndex, bool Success ) -> void
		{
			OnCompletion.ExecuteIfBound( SlotName, UserIndex, Success );
			
			if (Success)
				OnSaveGameCreated.Broadcast( UserIndex, SlotName, Header );
		} );

		Super::SaveDataToSlot_Async( WorldContext, Header, CheckpointData, FinalSlotName, UserIndex, OnCoreCompletion );
	});

	FillAsyncSaveGameData_Async( WorldContext, SaveData, true, AsyncFillComplete );
}

void UUpliftCampaignSaveUtilities::SaveCheckpointToSlot( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, FString SlotName, int32 UserIndex, ESaveGameType SaveType, const FText &DisplayName )
{
	check( IsInGameThread( ) );

	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
		return; // Ignore saving a developer save
	
	if (!ensureAlways( CheckpointData != nullptr ))
		return;
	if (!ensureAlways( CheckpointData->bCreationComplete == true ))
		return;

	ensureAlways( SaveType != ESaveGameType::Quick ); // this is probably an error as saving a checkpoint is automated, but quick should always be user triggered
	UpdateSlotName( WorldContext, SlotName, SaveType, CheckpointData->CampaignID );

	const auto SaveGameData = CastChecked< UUpliftCampaignSave >( StaticDuplicateObject( CheckpointData, GetTransientPackage( ) ) );
	SaveGameData->SaveType = SaveType;

	if (!ensureAlways( FillCheckpointData( WorldContext, SaveGameData ) ))
		return;
	SaveGameData->bCreationComplete = true;

	const auto Header = CreateSaveGameHeader( SaveGameData, SlotName, SaveType, SlotName );
	if (!ensureAlways( Header != nullptr ))
		return;

	const auto SaveDataResult = Super::SaveDataToSlot( WorldContext, Header, SaveGameData, SlotName, UserIndex );
	ensureAlways( SaveDataResult );
	
	if (SaveDataResult)
		OnSaveGameCreated.Broadcast( UserIndex, SlotName, Header );
}

void UUpliftCampaignSaveUtilities::SaveCheckpointToSlot_Async( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, FString SlotName, int32 UserIndex, ESaveGameType SaveType, const FText &DisplayName, const FSaveAsyncCallback &OnCompletion )
{
	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return; // Ignore saving a developer save
	}
	
	if (!ensureAlways( Super::SaveOperationsAreAllowed( ) ))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}

	if (!ensureAlways( !SlotName.IsEmpty( ) ))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}

	if (!ensureAlways( UserIndex >= 0 ))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}

	if (!ensureAlways( CheckpointData != nullptr ))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}

	if (!ensureAlways( CheckpointData->bCreationComplete ))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}

	ensureAlways( SaveType != ESaveGameType::Quick ); // this is probably an error as saving a checkpoint is automated, but quick should always be user triggered
	UpdateSlotName( WorldContext, SlotName, SaveType, CheckpointData->CampaignID );

	struct FSaveCheckpointTask : public FSaveDataTask
	{
		FSaveCheckpointTask( const FString &SN, int32 UI, ESaveGameType ST, const FText &DN, const UUpliftCampaignSave *CP ) : FSaveDataTask( UI ), SlotName( SN ), DisplayName( DN ), SaveType( ST ), CheckpointData( CP ) { }

		void Branch(const UObject *WorldContext) override
		{
			Context = WorldContext;
		}

		void DoWork( )
		{
			auto Parameters = InitStaticDuplicateObjectParams( CheckpointData.Get( ), GetTransientPackage( ) );
			Parameters.bSkipPostLoad = true; // we don't need any functionality from PostLoad and skipping allows us to run duplicate threaded
			
			SaveGameData = CastChecked< UUpliftCampaignSave >( StaticDuplicateObjectEx(Parameters) );
			if (SaveGameData == nullptr)
				return;
			SaveGameData->SaveType = SaveType;

			if (!FillCheckpointData( Context, SaveGameData ))
				return;
			SaveGameData->bCreationComplete = true;

			Header = CreateSaveGameHeader( SaveGameData, SlotName, SaveType, SlotName );
			if (Header == nullptr)
				return;

			bResult = Super::SaveDataToSlot_Internal( Context, Header, SaveGameData, SlotName, UserIndex );
		}

		void Join(const UObject *WorldContext) override
		{
			if (SaveGameData != nullptr)
				SaveGameData->ClearInternalFlags( EInternalObjectFlags::Async );
			if (Header != nullptr)
				Header->ClearInternalFlags( EInternalObjectFlags::Async );
		}

		// The name of the slot to save to
		FString SlotName;

		// The display name to assign to the save header
		FText DisplayName;

		// The type of save to write
		ESaveGameType SaveType;

		// The checkpoint to use as the source data for the save
		TStrongObjectPtr< const UUpliftCampaignSave > CheckpointData;

		// The actual save data to write to the disk
		UUpliftCampaignSave *SaveGameData = nullptr;

		// The associated header to write to the disk
		UUpliftCampaignSaveHeader *Header = nullptr;

		// The world context for filling the checkpoint data
		const UObject *Context = nullptr;
		
		// The overall result of the write operation
		bool bResult = false;
		
	} NewTask( SlotName, UserIndex, SaveType, DisplayName, CheckpointData );

	const auto AsyncTaskComplete = FAsyncTaskComplete< FSaveCheckpointTask >::CreateLambda( [ OnCompletion ]( const UObject *WorldContext, const FSaveCheckpointTask &Task )
	{
		OnCompletion.ExecuteIfBound( Task.SlotName, Task.UserIndex, Task.bResult );
		
		if (Task.bResult)
			OnSaveGameCreated.Broadcast( Task.UserIndex, Task.SlotName, Task.Header );
	});

	if (!Super::StartAsyncSaveTask( WorldContext, MoveTemp( NewTask ), "Save Checkpoint to Slot", AsyncTaskComplete ))
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
}

static void PostSaveGameLoad( const UObject *WorldContext, const UUpliftCampaignSaveHeader *Header, const UUpliftCampaignSave *SaveData, const FString &SlotName )
{
	const auto SaveSubsystem = UUpliftCampaignSaveSubsystem::GetSubsystem( WorldContext );
	check( SaveSubsystem != nullptr );

	SaveSubsystem->LastSaveSlotName = SlotName;

	UUpliftCampaignSaveUtilities::LoadCheckpointSave( WorldContext, SaveData );
}

void UUpliftCampaignSaveUtilities::LoadCheckpointSave( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData )
{
	const auto SaveSubsystem = UUpliftCampaignSaveSubsystem::GetSubsystem( WorldContext );
	check( SaveSubsystem != nullptr );

	SaveSubsystem->SaveGame = CheckpointData;

	const TArray< FName > Bundles;

	const auto ContentEntitlements = UFeatureContentManager::GetSubsystem( WorldContext );
	ContentEntitlements->SetEnabledFeatures( TSet( CheckpointData->ContentFeatures ), Bundles );

	UGameplayStatics::OpenLevel( WorldContext, CheckpointData->WorldToLoad.GetLongPackageFName( ), true );
}

ESaveDataLoadResult UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, const UUpliftCampaignSaveHeader *& outHeader, const UUpliftCampaignSave *& outSaveData )
{
	check( IsInGameThread( ) );
	
	const auto Header = NewObject< UUpliftCampaignSaveHeader >( GetTransientPackage( ) );
	const auto SaveData = NewObject< UUpliftCampaignSave >( GetTransientPackage( ) );

	outHeader = Header;
	outSaveData = SaveData;

	const auto Result = Super::LoadDataFromSlot( WorldContext, SlotName, UserIndex, Header, SaveData );
	if (Result != ESaveDataLoadResult::Success)
		return Result;

	SaveData->bCreationComplete = true;

	PostSaveGameLoad( WorldContext, Header, SaveData, SlotName );

	return ESaveDataLoadResult::Success;
}

void UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot_Async( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, const FLoadAsyncCallback &OnCompletion )
{
	check( OnCompletion.IsBound( ) );

	const TWeakObjectPtr< const UObject > WeakWorldContext( WorldContext );
	auto CompletionLambda = [ WeakWorldContext, OnCompletion ]( const FString &SlotName, int32 UserIndex, ESaveDataLoadResult Result, const USaveDataHeader *Header, const USaveData *SaveData ) -> void
	{
		const auto GameHeader = CastChecked< UUpliftCampaignSaveHeader >( Header, ECastCheckedType::NullAllowed );
		const auto GameSaveData = CastChecked< UUpliftCampaignSave >( SaveData, ECastCheckedType::NullAllowed );

		if (GameSaveData != nullptr)
			const_cast< UUpliftCampaignSave* >( GameSaveData )->bCreationComplete = true;

		if (const auto WorldContext = WeakWorldContext.Get( ))
		{
			OnCompletion.Execute( SlotName, UserIndex, Result, GameHeader, GameSaveData );

			if (Result == ESaveDataLoadResult::Success)
				PostSaveGameLoad( WorldContext, GameHeader, GameSaveData, SlotName );
		}
		else
		{
			ensureAlwaysMsgf( false, TEXT( "World Context for load save became invalid during async operation!" ) );
			OnCompletion.Execute( SlotName, UserIndex, ESaveDataLoadResult::SerializationFailed, nullptr, nullptr );
		}
	};

	const auto Header = NewObject< UUpliftCampaignSaveHeader >( GetTransientPackage( ) );
	const auto SaveData = NewObject< UUpliftCampaignSave >( GetTransientPackage( ) );

	Super::LoadDataFromSlot_Async( WorldContext, SlotName, UserIndex, Header, SaveData, FLoadAsyncCallback_Core::CreateLambda( CompletionLambda ) );
}

ESaveDataLoadResult UUpliftCampaignSaveUtilities::LoadSlotHeaderOnly( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, const UUpliftCampaignSaveHeader *& outHeader )
{
	check( IsInGameThread( ) );

	ESaveDataLoadResult Result = ESaveDataLoadResult::FailedToOpen;

	outHeader = Cast< UUpliftCampaignSaveHeader >( Super::LoadSlotHeaderOnly( WorldContext, SlotName, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), Result ) );

	return Result;
}

void UUpliftCampaignSaveUtilities::LoadSlotHeaderOnly_Async( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, const FLoadHeaderAsyncCallback &OnCompletion )
{
	check( OnCompletion.IsBound( ) );

	auto CompletionLambda = [ OnCompletion ]( const FString &SlotName, int32 UserIndex, ESaveDataLoadResult Result, const USaveDataHeader *Header ) -> void
	{
		OnCompletion.Execute( SlotName, UserIndex, Result, Cast< UUpliftCampaignSaveHeader >( Header ) );
	};

	Super::LoadSlotHeaderOnly_Async( WorldContext, SlotName, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), FLoadHeaderAsyncCallback_Core::CreateLambda( CompletionLambda ) );
}

bool UUpliftCampaignSaveUtilities::AnySavesExist( const UObject *WorldContext, int32 UserIndex, const FSaveFilter &Filter )
{
	if (!Filter.IsBound( ))
		return Super::AnySavesExist( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ) );

	auto FilterLambda = [ Filter ]( const FString &SlotName, int32 UserIndex, const USaveDataHeader *Header, ESaveDataLoadResult LoadingResult ) -> bool
	{
		const auto GameHeader = CastChecked< UUpliftCampaignSaveHeader >( Header );
		return Filter.Execute( SlotName, UserIndex, GameHeader, LoadingResult );
	};

	return Super::AnySavesExist( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), FSaveFilter_Core::CreateLambda( FilterLambda ) );
}

void UUpliftCampaignSaveUtilities::AnySavesExist_Async( const UObject *WorldContext, int32 UserIndex, const FSavesExistAsyncCallback &OnCompletion, const FSaveFilter &Filter )
{
	check( OnCompletion.IsBound( ) );

	auto CompletionLambda = [ OnCompletion ]( bool Success ) -> void
	{
		OnCompletion.Execute( Success );
	};
	const auto OnCompletion_Core = Super::FSavesExistAsyncCallback_Core::CreateLambda( CompletionLambda );

	if (!Filter.IsBound( ))
	{
		Super::AnySavesExist_Async( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), OnCompletion_Core );
		return;
	}

	auto FilterLambda = [ Filter ]( const FString &SlotName, int32 UserIndex, const USaveDataHeader *Header, ESaveDataLoadResult LoadingResult ) -> bool
	{
		const auto GameHeader = CastChecked< UUpliftCampaignSaveHeader >( Header );
		return Filter.Execute( SlotName, UserIndex, GameHeader, LoadingResult );
	};

	Super::AnySavesExist_Async( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), OnCompletion_Core, FSaveFilter_Core::CreateLambda( FilterLambda ) );
}

TArray< FEnumeratedSaveGameHeader > UUpliftCampaignSaveUtilities::EnumerateSaveHeaders( const UObject *WorldContext, int32 UserIndex, const FSaveFilter &Filter )
{
	TArray< FEnumeratedHeader_Core > CoreResults;
	if (Filter.IsBound( ))
	{
		auto FilterLambda = [ Filter ]( const FString &SlotName, int32 UserIndex, const USaveDataHeader *Header, ESaveDataLoadResult LoadingResult ) -> bool
		{
			const auto GameHeader = CastChecked< UUpliftCampaignSaveHeader >( Header );
			return Filter.Execute( SlotName, UserIndex, GameHeader, LoadingResult );
		};

		CoreResults = Super::EnumerateSaveHeaders( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), FSaveFilter_Core::CreateLambda( FilterLambda ) );
	}
	else
	{
		CoreResults = Super::EnumerateSaveHeaders( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ) );
	}

	if (CoreResults.Num( ) == 0)
		return { };

	TArray< FEnumeratedSaveGameHeader > Results;
	Results.Reserve( CoreResults.Num( ) );
	for (const auto &R : CoreResults)
		Results.Emplace( R );

	return Results;
}

void UUpliftCampaignSaveUtilities::EnumerateSaveHeaders_Async( const UObject *WorldContext, int32 UserIndex, const FEnumerateHeadersComplete &OnCompletion, const FSaveFilter &Filter, const FLoadHeaderAsyncCallback &OnSingleHeader )
{
	check( OnCompletion.IsBound( ) );

	auto CompletionLambda = [ OnCompletion ]( const TArray< FEnumeratedHeader_Core > &CoreResults ) -> void
	{
		TArray< FEnumeratedSaveGameHeader > Results;
		Results.Reserve( CoreResults.Num( ) );
		for (const auto &R : CoreResults)
			Results.Emplace( R );

		OnCompletion.Execute( Results );
	};
	const auto OnCompletion_Core = Super::FEnumerateHeadersComplete_Core::CreateLambda( CompletionLambda );

	auto OnSingleHeader_Core = Super::FLoadHeaderAsyncCallback_Core::CreateLambda( [ OnSingleHeader ]( const FString &SlotName, int32 UserIndex, ESaveDataLoadResult Result, const USaveDataHeader *Header )
	{
		OnSingleHeader.ExecuteIfBound( SlotName, UserIndex, Result, Cast< UUpliftCampaignSaveHeader >( Header ) );		
	} );

	if (!Filter.IsBound( ))
	{
		Super::EnumerateSaveHeaders_Async( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), OnCompletion_Core );
		return;
	}

	auto FilterLambda = [ Filter ]( const FString &SlotName, int32 UserIndex, const USaveDataHeader *Header, ESaveDataLoadResult LoadingResult ) -> bool
	{
		const auto GameHeader = CastChecked< UUpliftCampaignSaveHeader >( Header, ECastCheckedType::NullAllowed );
		return Filter.Execute( SlotName, UserIndex, GameHeader, LoadingResult );
	};

	Super::EnumerateSaveHeaders_Async( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), OnCompletion_Core, FSaveFilter_Core::CreateLambda( FilterLambda ) );
}

const UUpliftCampaignSave* UUpliftCampaignSaveUtilities::CreateTravelSave( const UObject *WorldContext, const TSoftObjectPtr< const UWorld > &Destination )
{
	const auto Save = CreateAndFillSaveData( WorldContext, { }, false, ESaveGameType::Travel );

	Save->WorldToLoad = Destination;

	return Save;
}

const UUpliftCampaignSave* UUpliftCampaignSaveUtilities::CreateCheckpointSave( const UObject *WorldContext )
{
	TArray< FString > BlockedReasons;
	if (IsSaveTypeBlocked( WorldContext, UUpliftCampaignSave::StaticClass( ), &BlockedReasons ))
	{
		UE_LOG( LogStarfireSaveData, Log, TEXT( "Request to save blocked:" ) );

		for (const auto &Reason : BlockedReasons)
			UE_LOG( LogStarfireSaveData, Log, TEXT( "\t\t%s" ), *Reason );

		return nullptr;
	}
	
	return CreateAndFillSaveData( WorldContext, { }, false, ESaveGameType::Checkpoint );
}

void UUpliftCampaignSaveUtilities::CreateCheckpointSave_Async( const UObject *WorldContext, const FCreateCheckpointComplete &OnCompletion )
{
	check( OnCompletion.IsBound( ) );

	TArray< FString > BlockedReasons;
	if (IsSaveTypeBlocked( WorldContext, UUpliftCampaignSave::StaticClass( ), &BlockedReasons ))
	{
		UE_LOG( LogStarfireSaveData, Log, TEXT( "Request to save blocked:" ) );

		for (const auto &Reason : BlockedReasons)
			UE_LOG( LogStarfireSaveData, Log, TEXT( "\t\t%s" ), *Reason );

		OnCompletion.Execute( WorldContext, nullptr, false );
		return;
	}

	const auto CheckpointData = CreateSaveData( WorldContext, { }, ESaveGameType::Checkpoint );

	FillAsyncSaveGameData_Async( WorldContext, CheckpointData, false, OnCompletion );
}

bool UUpliftCampaignSaveUtilities::AutoSave( const UObject *WorldContext, FString SlotName, int32 UserIndex, const FText &DisplayName )
{
	if (!ensureAlways( !AnyAsyncSaveTasksPending( WorldContext )))
		return false;
	
	UpdateSlotName( WorldContext, SlotName, ESaveGameType::Auto, ADS_Campaign::GetCampaignID( WorldContext ) );

	return SaveToSlot( WorldContext, SlotName, UserIndex, ESaveGameType::Auto, DisplayName );
}

void UUpliftCampaignSaveUtilities::AutoSave_Async( const UObject *WorldContext, FString SlotName, int32 UserIndex, const FText &DisplayName, const FSaveAsyncCallback &OnCompletion )
{
	if (!ensureAlways( SaveOperationsAreAllowed( ) ))
	{
		OnCompletion.ExecuteIfBound( FString( ), 0, false );
		return;
	}

	if (!ensureAlways( UserIndex >= 0 ))
	{
		OnCompletion.ExecuteIfBound( FString( ), 0, false );
		return;
	}

	UpdateSlotName( WorldContext, SlotName, ESaveGameType::Auto, ADS_Campaign::GetCampaignID( WorldContext ) );

	struct FAutoSaveTask : public FSaveDataTask
	{
		FAutoSaveTask( int UI, const FString &SN, const FText &DN ) : FSaveDataTask( UI ), DisplayName( DN ), SlotName( SN ) { }

		// Begin the multiple async task required for creating an async auto save - creating the save and determining the name
		void Branch(const UObject *WorldContext) override
		{
			Context = WorldContext;

			SaveData = TStrongObjectPtr( CreateSaveData( WorldContext, DisplayName, ESaveGameType::Auto ) );
		}

		void DoWork( void )
		{
			TRACE_CPUPROFILER_EVENT_SCOPE( TEXT( "FAutoSaveTask::DoWork" ) )

			bResult = FillAsyncSaveGameData( SaveData.Get( ) );

			if (!bResult)
				return; // something went wrong so quit

			FGCScopeGuard GCGuard;

			Header = CreateSaveGameHeader( SaveData.Get( ), SlotName, ESaveGameType::Auto, SlotName );
			if (Header == nullptr)
			{
				bResult = false;
				return;
			}

			bResult = SaveDataToSlot_Internal( Context, Header, SaveData.Get( ), SlotName, UserIndex );
		}

		void Join(const UObject *WorldContext) override
		{
			if (Header != nullptr) // we allocated the header during the async process, so clear this flag to allow it to be GC'd
				Header->ClearInternalFlags( EInternalObjectFlags::Async );
		}

		// The save game data to be saved into the auto save slot
		TStrongObjectPtr< UUpliftCampaignSave > SaveData;

		// The header metadata about the save
		UUpliftCampaignSaveHeader *Header = nullptr;
		
		// The user facing name that should be used for this save
		FText DisplayName;
		
		// The slot that the save data should be written to
		FString SlotName;

		// The ultimate outcome of this async task
		bool bResult = true;

		// The context in which the operation is running
		const UObject *Context = nullptr;

	} NewTask( UserIndex, SlotName, DisplayName );

	const auto AsyncComplete = FAsyncTaskComplete< FAutoSaveTask >::CreateLambda( [ OnCompletion ]( const UObject *WorldContext, const FAutoSaveTask &Task )
	{
		OnCompletion.ExecuteIfBound( Task.SlotName, Task.UserIndex, Task.bResult );
		
		if (Task.bResult)
			OnSaveGameCreated.Broadcast( Task.UserIndex, Task.SlotName, Task.Header );
	});

	if (!StartAsyncSaveTask( WorldContext, MoveTemp( NewTask ), "Auto Save", AsyncComplete ))
		OnCompletion.ExecuteIfBound( FString( ), UserIndex, false );
}

bool UUpliftCampaignSaveUtilities::QuickSave( const UObject *WorldContext, int32 UserIndex )
{
	if (!IsManualSavingAllowed( WorldContext ))
		return false;
	
	const auto CampaignID = ADS_Campaign::GetCampaignID( WorldContext );

	return SaveToSlot( WorldContext, GetQuickSaveSlotName( CampaignID ), UserIndex, ESaveGameType::Quick );
}

void UUpliftCampaignSaveUtilities::QuickSave_Async( const UObject *WorldContext, int32 UserIndex, const FSaveAsyncCallback &OnCompletion )
{
	const auto CampaignID = ADS_Campaign::GetCampaignID( WorldContext );
	const auto SlotName = GetQuickSaveSlotName( CampaignID );
	
	if (!IsManualSavingAllowed( WorldContext ))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}
	
	SaveToSlot_Async( WorldContext, SlotName, UserIndex, ESaveGameType::Quick, { }, OnCompletion );
}

bool UUpliftCampaignSaveUtilities::SaveToPath( const UObject *WorldContext, const FString &PathName, ESaveGameType SaveType, const FText &DisplayName )
{
	check( IsInGameThread( ) );

	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
		return false; // Ignore saving a developer save

	TArray< FString > BlockedReasons;
	if (IsSaveTypeBlocked( WorldContext, UUpliftCampaignSave::StaticClass( ), &BlockedReasons ))
	{
		UE_LOG( LogStarfireSaveData, Log, TEXT( "Request to save blocked:" ) );

		for (const auto &Reason : BlockedReasons)
			UE_LOG( LogStarfireSaveData, Log, TEXT( "\t\t%s" ), *Reason );

		return false;
	}

	const auto SaveData = CreateAndFillSaveData( WorldContext, DisplayName, true, SaveType );
	if (!ensureAlways( SaveData != nullptr ))
		return false;

	int Index = INDEX_NONE;
	PathName.FindLastChar( '/', Index );

	const auto SlotName = PathName.Right( PathName.Len( ) - Index - 1 );

	const auto Header = CreateSaveGameHeader( SaveData, SlotName, SaveType, SlotName );
	if (!ensureAlways( Header != nullptr ))
		return false;

	return Super::SaveDataToPath( WorldContext, Header, SaveData, PathName );
}

void UUpliftCampaignSaveUtilities::SaveToPath_Async( const UObject *WorldContext, const FString &PathName, ESaveGameType SaveType, const FText &DisplayName, const FSaveAsyncCallback &OnCompletion )
{
	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
	{
		OnCompletion.ExecuteIfBound( PathName, -1, false );
		return; // Ignore saving a developer save
	}

	TArray< FString > BlockedReasons;
	if (IsSaveTypeBlocked( WorldContext, UUpliftCampaignSave::StaticClass( ), &BlockedReasons ))
	{
		UE_LOG( LogStarfireSaveData, Log, TEXT( "Request to save blocked:" ) );

		for (const auto &Reason : BlockedReasons)
			UE_LOG( LogStarfireSaveData, Log, TEXT( "\t\t%s" ), *Reason );

		OnCompletion.ExecuteIfBound( PathName, -1, false );
		return;
	}

	const auto SaveData = CreateSaveData( WorldContext, DisplayName, SaveType );
	if (!ensureAlways( SaveData != nullptr ))
	{
		OnCompletion.ExecuteIfBound( PathName, -1, false );
		return;
	}

	int Index = INDEX_NONE;
	PathName.FindLastChar( '/', Index );

	const auto SlotName = PathName.Right( PathName.Len( ) - Index - 1 );

	const auto AsyncFillComplete = FCreateCheckpointComplete::CreateLambda( [ PathName, SaveType, SlotName, OnCompletion ]( const UObject *WorldContext, const UUpliftCampaignSave* CheckpointData, bool Success )
	{
		if (!Success)
		{
			OnCompletion.ExecuteIfBound( PathName, -1, false );
			return;
		}

		const UUpliftCampaignSaveHeader* Header = CreateSaveGameHeader( CheckpointData, SlotName, SaveType, SlotName );
		if (!ensureAlways( Header != nullptr ))
		{
			OnCompletion.ExecuteIfBound( PathName, -1, false );
			return;
		}

		SaveDataToPath_Async( WorldContext, Header, CheckpointData, PathName, OnCompletion );
	});

	FillAsyncSaveGameData_Async( WorldContext, SaveData, true, AsyncFillComplete );
}

void UUpliftCampaignSaveUtilities::SaveCheckpointToPath( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, const FString &PathName, ESaveGameType SaveType )
{
	check( IsInGameThread( ) );

	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
		return; // Ignore saving a developer save
	
	if (!ensureAlways( CheckpointData != nullptr ))
		return;
	if (!ensureAlways( CheckpointData->bCreationComplete == true ))
		return;

	int Index = INDEX_NONE;
	PathName.FindLastChar( '/', Index );

	const auto SlotName = PathName.Right( PathName.Len( ) - Index - 1 );

	const auto SaveGameData = CastChecked< UUpliftCampaignSave >( StaticDuplicateObject( CheckpointData, GetTransientPackage( ) ) );
	SaveGameData->SaveType = SaveType;

	if (!ensureAlways( FillCheckpointData( WorldContext, SaveGameData ) ))
		return;
	SaveGameData->bCreationComplete = true;

	const auto Header = CreateSaveGameHeader( SaveGameData, SlotName, SaveType, SlotName );
	if (!ensureAlways( Header != nullptr ))
		return;

	const auto SaveDataResult = Super::SaveDataToPath( WorldContext, Header, SaveGameData, PathName );
	ensureAlways( SaveDataResult );
}

void UUpliftCampaignSaveUtilities::SaveCheckpointToPath_Async( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, const FString &PathName, ESaveGameType SaveType, const FSaveAsyncCallback &OnCompletion )
{
	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
	{
		OnCompletion.ExecuteIfBound( PathName, -1, false );
		return; // Ignore saving a developer save
	}
	
	if (!ensureAlways( Super::SaveOperationsAreAllowed( ) ))
	{
		OnCompletion.ExecuteIfBound( PathName, -1, false );
		return;
	}

	if (!ensureAlways( !PathName.IsEmpty( ) ))
	{
		OnCompletion.ExecuteIfBound( PathName, -1, false );
		return;
	}

	if (!ensureAlways( CheckpointData != nullptr ))
	{
		OnCompletion.ExecuteIfBound( PathName, -1, false );
		return;
	}

	if (!ensureAlways( CheckpointData->bCreationComplete ))
	{
		OnCompletion.ExecuteIfBound( PathName, -1, false );
		return;
	}

	int Index = INDEX_NONE;
	PathName.FindLastChar( '/', Index );

	const auto SlotName = PathName.Right( PathName.Len( ) - Index - 1 );

	struct FSaveCheckpointTask : public FSaveDataTask
	{
		FSaveCheckpointTask( const FString &PN, ESaveGameType ST, const FString &SN, const UUpliftCampaignSave *CP ) : FSaveDataTask( -1 ), PathName( PN ), SlotName( SN ), SaveType( ST ), CheckpointData( CP ) { }

		void Branch(const UObject *WorldContext) override
		{
			Context = WorldContext;
		}

		void DoWork( )
		{
			SaveGameData = CastChecked< UUpliftCampaignSave >( StaticDuplicateObject( CheckpointData.Get( ), GetTransientPackage( ) ) );
			if (SaveGameData == nullptr)
				return;
			SaveGameData->SaveType = SaveType;

			if (!FillCheckpointData( Context, SaveGameData ))
				return;
			SaveGameData->bCreationComplete = true;

			Header = CreateSaveGameHeader( SaveGameData, SlotName, SaveType, SlotName );
			if (Header == nullptr)
				return;

			bResult = Super::SaveDataToPath_Internal( Context, Header, SaveGameData, PathName );
		}

		void Join(const UObject *WorldContext) override
		{
			if (SaveGameData != nullptr)
				SaveGameData->ClearInternalFlags( EInternalObjectFlags::Async );
			if (Header != nullptr)
				Header->ClearInternalFlags( EInternalObjectFlags::Async );
		}

		// The name of the slot to save to
		FString PathName;

		// The display name to assign to the save header
		FString SlotName;

		// The type of save to write
		ESaveGameType SaveType;

		// The checkpoint to use as the source data for the save
		TStrongObjectPtr< const UUpliftCampaignSave > CheckpointData;

		// The actual save data to write to the disk
		UUpliftCampaignSave *SaveGameData = nullptr;

		// The associated header to write to the disk
		UUpliftCampaignSaveHeader *Header = nullptr;

		// The world context for filling the checkpoint data
		const UObject *Context = nullptr;

		// The overall result of the write operation
		bool bResult = false;

	} NewTask( PathName, SaveType, SlotName, CheckpointData );

	const auto AsyncTaskComplete = FAsyncTaskComplete< FSaveCheckpointTask >::CreateLambda( [ OnCompletion ]( const UObject *WorldContext, const FSaveCheckpointTask &Task )
	{
		OnCompletion.ExecuteIfBound( Task.PathName, Task.UserIndex, Task.bResult );
	});

	if (!Super::StartAsyncSaveTask( WorldContext, MoveTemp( NewTask ), "Save Checkpoint to Path", AsyncTaskComplete ))
		OnCompletion.ExecuteIfBound( PathName, -1, false );
}

ESaveDataLoadResult UUpliftCampaignSaveUtilities::LoadSaveGameFromPath( const UObject *WorldContext, const FString &PathName, const UUpliftCampaignSaveHeader *& outHeader, const UUpliftCampaignSave *& outSaveData )
{
	check( IsInGameThread( ) );

	const auto Header = NewObject< UUpliftCampaignSaveHeader >( GetTransientPackage( ) );
	const auto SaveData = NewObject< UUpliftCampaignSave >( GetTransientPackage( ) );

	outHeader = Header;
	outSaveData = SaveData;

	const auto Result = Super::LoadDataFromPath( WorldContext, PathName, Header, SaveData );
	if (Result != ESaveDataLoadResult::Success)
		return Result;

	SaveData->bCreationComplete = true;

	PostSaveGameLoad( WorldContext, Header, SaveData, PathName );

	return ESaveDataLoadResult::Success;
}

void UUpliftCampaignSaveUtilities::LoadSaveGameFromPath_Async( const UObject *WorldContext, const FString &PathName, const FLoadAsyncCallback &OnCompletion )
{
	check( OnCompletion.IsBound( ) );

	const TWeakObjectPtr< const UObject > WeakWorldContext( WorldContext );
	auto CompletionLambda = [ WeakWorldContext, OnCompletion ]( const FString &SlotName, int32 UserIndex, ESaveDataLoadResult Result, const USaveDataHeader *Header, const USaveData *SaveData ) -> void
	{
		const auto GameHeader = CastChecked< UUpliftCampaignSaveHeader >( Header, ECastCheckedType::NullAllowed );
		const auto GameSaveData = CastChecked< UUpliftCampaignSave >( SaveData, ECastCheckedType::NullAllowed );

		if (GameSaveData != nullptr)
			const_cast< UUpliftCampaignSave* >( GameSaveData )->bCreationComplete = true;

		if (const auto WorldContext = WeakWorldContext.Get( ))
		{
			OnCompletion.Execute( SlotName, UserIndex, Result, GameHeader, GameSaveData );

			if (Result == ESaveDataLoadResult::Success)
				PostSaveGameLoad( WorldContext, GameHeader, GameSaveData, SlotName );
		}
		else
		{
			ensureAlwaysMsgf( false, TEXT( "World Context for load save became invalid during async operation!" ) );
			OnCompletion.Execute( SlotName, UserIndex, ESaveDataLoadResult::SerializationFailed, nullptr, nullptr );
		}
	};

	const auto Header = NewObject< UUpliftCampaignSaveHeader >( GetTransientPackage( ) );
	const auto SaveData = NewObject< UUpliftCampaignSave >( GetTransientPackage( ) );

	Super::LoadDataFromPath_Async( WorldContext, PathName, Header, SaveData, FLoadAsyncCallback_Core::CreateLambda( CompletionLambda ) );
}

ESaveDataLoadResult UUpliftCampaignSaveUtilities::LoadPathHeaderOnly( const UObject *WorldContext, const FString &PathName, const UUpliftCampaignSaveHeader *& outHeader )
{
	check( IsInGameThread( ) );

	ESaveDataLoadResult Result = ESaveDataLoadResult::FailedToOpen;

	outHeader = Cast< UUpliftCampaignSaveHeader >( Super::LoadPathHeaderOnly( WorldContext, PathName, UUpliftCampaignSaveHeader::StaticClass( ), Result ) );

	return Result;
}

void UUpliftCampaignSaveUtilities::LoadPathHeaderOnly_Async( const UObject *WorldContext, const FString &PathName, const FLoadHeaderAsyncCallback &OnCompletion )
{
	check( OnCompletion.IsBound( ) );

	auto CompletionLambda = [ OnCompletion ]( const FString &SlotName, int32 UserIndex, ESaveDataLoadResult Result, const USaveDataHeader *Header ) -> void
	{
		OnCompletion.Execute( SlotName, UserIndex, Result, Cast< UUpliftCampaignSaveHeader >( Header ) );
	};

	Super::LoadPathHeaderOnly_Async( WorldContext, PathName, UUpliftCampaignSaveHeader::StaticClass( ), FLoadHeaderAsyncCallback_Core::CreateLambda( CompletionLambda ) );
}

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
bool UUpliftCampaignSaveUtilities::DeveloperSave( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, const FText &DisplayName )
{
	if (!ensureAlways( !SlotName.IsEmpty( ) ))
		return false;

	TArray< FString > BlockedReasons;
	if (IsSaveTypeBlocked( WorldContext, UUpliftCampaignSave::StaticClass( ), &BlockedReasons ))
	{
		UE_LOG( LogStarfireSaveData, Log, TEXT( "Request to save blocked:" ) );

		for (const auto &Reason : BlockedReasons)
			UE_LOG( LogStarfireSaveData, Log, TEXT( "\t\t%s" ), *Reason );

		return false;
	}

	return SaveToSlot( WorldContext, DevSavePrefix + SlotName, UserIndex, ESaveGameType::Developer, DisplayName );
}

void UUpliftCampaignSaveUtilities::DeveloperSave_Async( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, const FText &DisplayName, const FSaveAsyncCallback &OnCompletion )
{
	if (!ensureAlways( !SlotName.IsEmpty( ) ))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}

	TArray< FString > BlockedReasons;
	if (IsSaveTypeBlocked( WorldContext, UUpliftCampaignSave::StaticClass( ), &BlockedReasons ))
	{
		UE_LOG( LogStarfireSaveData, Log, TEXT( "Request to save blocked:" ) );

		for (const auto &Reason : BlockedReasons)
			UE_LOG( LogStarfireSaveData, Log, TEXT( "\t\t%s" ), *Reason );

		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}

	SaveToSlot_Async( WorldContext, DevSavePrefix + SlotName, UserIndex, ESaveGameType::Developer, DisplayName, OnCompletion );
}
#endif