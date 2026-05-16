// Copyright Russell Aasland. All Rights Reserved.

#include "SaveGames/UpliftCampaignSaveUtilities.h"

#include "SaveGames/UpliftCampaignSaveHeader.h"
#include "SaveGames/UpliftCampaignSaveGame.h"
#include "SaveGames/UpliftCampaignSaveSubsystem.h"

#include "GameFeatures/FeatureContentManager.h"

// Engine
#include "Kismet/GameplayStatics.h"

// Core
#include "GameWorld/CampaignGameMode.h"
#include "UObject/GarbageCollection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftCampaignSaveUtilities)

static const FString AutoSavePrefix = "AutoSave_";
extern const FString DevSavePrefix = "Dev_"; // extern'd to GameSaveGameBlueprintUtilities

static TAutoConsoleVariable< bool > CVar_AllowDeveloperSaves( TEXT( "Uplift.SaveGames.AllowDeveloperSaves" ),
	#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	true,
	#else
		false,
	#endif
	TEXT( "Whether or not developer save request should be honored" ), ECVF_Cheat );

static TAutoConsoleVariable< int > CVar_MaxAutoSaveSlots( TEXT( "Uplift.SaveGame.MaxAutoSaveSlots" ), 5, TEXT( "The maximum number of unique auto-saves allowed at one time" ), ECVF_Cheat );
static TAutoConsoleVariable< FString > CVar_QuickSaveSlotName( TEXT( "Uplift.SaveGames.QuickSaveSlotName" ), "QuickSave", TEXT( "The name of the slot to use for savegames" ), ECVF_Cheat );

FString GetQuickSaveSlotName( void ) { return CVar_QuickSaveSlotName.GetValueOnAnyThread( ); }
FString GetQuickSaveDisplayName( void ) { return NSLOCTEXT( "Uplift_SaveGames", "QuickSaveFriendlyName", "Quick Save" ).ToString( ); }

void UpdateSlotName( FString& SlotName, ESaveGameType SaveType )
{
	switch (SaveType)
	{
		case ESaveGameType::Auto: SlotName = AutoSavePrefix + SlotName;
			break;
		case ESaveGameType::Developer: SlotName = DevSavePrefix + SlotName;
			break;

		default: // other types don't modify the slot name
			break;
	}
}

FEnumeratedSaveGameHeader::FEnumeratedSaveGameHeader( const USaveDataUtilities::FEnumeratedHeader_Core &Core ) :
	SlotName( Core.SlotName ),
	LoadingResult( Core.LoadingResult ),
	Header( CastChecked< UUpliftCampaignSaveHeader >( Core.Header, ECastCheckedType::NullAllowed ) )
{
}

TArray< FString > UUpliftCampaignSaveUtilities::EnumerateSlotNames( int32 UserIndex )
{
	return Super::EnumerateSlotNames( UserIndex );
}

FString UUpliftCampaignSaveUtilities::GetUnusedSlotName( int32 UserIndex, ESaveGameType SaveType )
{
	ensureAlways( SaveType != ESaveGameType::User ); // User saves should not have to find an unused slot name
	
	static const TArray< FString > Types = { FString( ), FString( ), AutoSavePrefix, DevSavePrefix };

	return Super::GetUnusedSlotName( UserIndex, Types[ (int)SaveType ] );
}

void UUpliftCampaignSaveUtilities::CacheSaveGameHeaders( const UObject *WorldContext, int UserIndex )
{
	Super::CacheAllSaveGameHeaders( WorldContext, UUpliftCampaignSaveHeader::StaticClass( ), UserIndex );
}

bool UUpliftCampaignSaveUtilities::DeleteSaveGameInSlot( const UObject *WorldContext, const FString &SlotName, int32 UserIndex )
{
	return Super::DeleteSaveGameInSlot( WorldContext, SlotName, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ) );
}

bool UUpliftCampaignSaveUtilities::DoesSaveGameExist( const FString &SlotName, int32 UserIndex )
{
	return Super::DoesSaveGameExist( SlotName, UserIndex );
}

bool UUpliftCampaignSaveUtilities::IsManualSavingAllowed( const UObject *WorldContext )
{
	const auto World = GEngine->GetWorldFromContextObject( WorldContext, EGetWorldErrorMode::LogAndReturnNull );
	if (World == nullptr)
		return false;

	const auto CampaignGameMode = Cast< ACampaignGameMode >( UGameplayStatics::GetGameMode( WorldContext ) );
	if (CampaignGameMode == nullptr)
		return false;

	return CampaignGameMode->IsManualSavingAllowed( );
}

bool UUpliftCampaignSaveUtilities::SaveToSlot( const UObject *WorldContext, FString SlotName, int32 UserIndex, ESaveGameType SaveType, FString DisplayNameOverride )
{
	check( IsInGameThread( ) );

	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
		return false; // Ignore saving a developer save
	
	const auto SaveData = CreateAndFillSaveData( WorldContext, true, false );
	if (!ensureAlways( SaveData != nullptr ))
		return false;

	if (DisplayNameOverride.IsEmpty( ))
		DisplayNameOverride = SlotName.Replace( TEXT( "_" ), TEXT( " " ) );

	UpdateSlotName( SlotName, SaveType );

	const auto Header = CreateSaveGameHeader( SaveData, SaveType, DisplayNameOverride );
	if (!ensureAlways( Header != nullptr ))
		return false;

	return Super::SaveDataToSlot( WorldContext, Header, SaveData, SlotName, UserIndex );
}

void UUpliftCampaignSaveUtilities::SaveToSlot_Async( const UObject *WorldContext, FString SlotName, int32 UserIndex, ESaveGameType SaveType, FString DisplayNameOverride, const FSaveAsyncCallback &OnCompletion )
{
	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return; // Ignore saving a developer save
	}

	const auto SaveData = CreateSaveData( WorldContext );
	if (!ensureAlways( SaveData != nullptr ))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}

	if (DisplayNameOverride.IsEmpty( ))
		DisplayNameOverride = SlotName.Replace( TEXT( "_" ), TEXT( " " ) );

	const auto AsyncFillComplete = FCreateCheckpointComplete::CreateLambda( [ SlotName, UserIndex, SaveType, DisplayNameOverride, OnCompletion ]( const UObject *WorldContext, const UUpliftCampaignSave* CheckpointData, bool Success )
	{
		if (!Success)
		{
			OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
			return;
		}

		FString FinalSlotName = SlotName;
		UpdateSlotName( FinalSlotName, SaveType );		

		const UUpliftCampaignSaveHeader* Header = CreateSaveGameHeader( CheckpointData, SaveType, DisplayNameOverride );
		if (!ensureAlways( Header != nullptr ))
		{
			OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
			return;
		}

		SaveDataToSlot_Async( WorldContext, Header, CheckpointData, FinalSlotName, UserIndex, OnCompletion );
	});

	FillAsyncSaveGameData_Async( WorldContext, SaveData, true, AsyncFillComplete );
}

void UUpliftCampaignSaveUtilities::SaveCheckpointToSlot( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, FString SlotName, int32 UserIndex, ESaveGameType SaveType, FString DisplayNameOverride )
{
	check( IsInGameThread( ) );

	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
		return; // Ignore saving a developer save
	
	if (!ensureAlways( CheckpointData != nullptr ))
		return;
	if (!ensureAlways( CheckpointData->bCreationComplete == true ))
		return;

	if (DisplayNameOverride.IsEmpty( ))
		DisplayNameOverride = SlotName.Replace( TEXT( "_" ), TEXT( " " ) );

	ensureAlways( SaveType != ESaveGameType::Quick ); // this is probably an error as saving a checkpoint is automated, but quick should always be user triggered
	UpdateSlotName( SlotName, SaveType );

	const auto SaveGameData = CastChecked< UUpliftCampaignSave >( StaticDuplicateObject( CheckpointData, GetTransientPackage( ) ) );

	if (!ensureAlways( FillCheckpointData( WorldContext, SaveGameData ) ))
		return;
	SaveGameData->bCreationComplete = true;

	const auto Header = CreateSaveGameHeader( SaveGameData, SaveType, DisplayNameOverride );
	if (!ensureAlways( Header != nullptr ))
		return;

	const auto SaveDataResult = Super::SaveDataToSlot( WorldContext, Header, SaveGameData, SlotName, UserIndex );
	ensureAlways( SaveDataResult );
}

void UUpliftCampaignSaveUtilities::SaveCheckpointToSlot_Async( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, FString SlotName, int32 UserIndex, ESaveGameType SaveType, FString DisplayNameOverride, const FSaveAsyncCallback &OnCompletion )
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

	if (DisplayNameOverride.IsEmpty( ))
		DisplayNameOverride = SlotName.Replace( TEXT( "_" ), TEXT( " " ) );

	ensureAlways( SaveType != ESaveGameType::Quick ); // this is probably an error as saving a checkpoint is automated, but quick should always be user triggered
	UpdateSlotName( SlotName, SaveType );

	struct FSaveCheckpointTask : public FSaveDataTask
	{
		FSaveCheckpointTask( const FString &SN, int32 UI, ESaveGameType ST, const FString &DN, const UUpliftCampaignSave *CP ) : FSaveDataTask( UI ), SlotName( SN ), DisplayName( DN ), SaveType( ST ), CheckpointData( CP ) { }

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

			if (!FillCheckpointData( Context, SaveGameData ))
				return;
			SaveGameData->bCreationComplete = true;

			Header = CreateSaveGameHeader( SaveGameData, SaveType, DisplayName );
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
		FString DisplayName;

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
		
	} NewTask( SlotName, UserIndex, SaveType, DisplayNameOverride, CheckpointData );

	const auto AsyncTaskComplete = FAsyncTaskComplete< FSaveCheckpointTask >::CreateLambda( [ OnCompletion ]( const UObject *WorldContext, const FSaveCheckpointTask &Task )
	{
		OnCompletion.ExecuteIfBound( Task.SlotName, Task.UserIndex, Task.bResult );
	});

	if (!Super::StartAsyncSaveTask( WorldContext, MoveTemp( NewTask ), "Save Checkpoint to Slot", AsyncTaskComplete ))
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
}

static void PostSaveGameLoad( const UObject *WorldContext, const UUpliftCampaignSaveHeader *Header, const UUpliftCampaignSave *SaveData, const FString &SlotName )
{
	const auto SaveSubsystem = UUpliftCampaignSaveSubsystem::Get( WorldContext );
	check( SaveSubsystem != nullptr );

	SaveSubsystem->LastSaveSlotName = SlotName;

	UUpliftCampaignSaveUtilities::LoadCheckpointSave( WorldContext, SaveData );
}

void UUpliftCampaignSaveUtilities::LoadCheckpointSave( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData )
{
	const auto SaveSubsystem = UUpliftCampaignSaveSubsystem::Get( WorldContext );
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

bool UUpliftCampaignSaveUtilities::LoadMostRecentSave( const UObject *WorldContext, int32 UserIndex, FString &outSlotName, const UUpliftCampaignSaveHeader *& outHeader, const UUpliftCampaignSave *& outSaveData, const FSaveFilter &Filter )
{
	if (!ensureAlways( WorldContext != nullptr ))
		return false;
	if (!ensureAlways( UserIndex >= 0 ))
		return false;

	const auto Results = FindMostRecentSave( WorldContext, UserIndex, Filter );
	if (Results.LoadingResult != ESaveDataLoadResult::Success)
		return false;

	outSlotName = Results.SlotName;

	return LoadSaveGameFromSlot( WorldContext, outSlotName, UserIndex, outHeader, outSaveData ) == ESaveDataLoadResult::Success;
}

void UUpliftCampaignSaveUtilities::LoadMostRecentSave_Async( const UObject *WorldContext, int32 UserIndex, const FLoadAsyncCallback &OnCompletion, const FSaveFilter &Filter )
{
	check( OnCompletion.IsBound( ) );

	if (!ensureAlways( UserIndex >= 0 ))
	{
		OnCompletion.Execute( FString( ), UserIndex, ESaveDataLoadResult::RequestFailure, nullptr, nullptr );
		return;
	}

	const TWeakObjectPtr< const UObject > WeakWorldContext( WorldContext );
	auto OnFindComplete = [ OnCompletion, WeakWorldContext ]( const FString &SlotName, int32 UserIndex, ESaveDataLoadResult Result, const UUpliftCampaignSaveHeader *Header ) -> void
	{
		if (Result != ESaveDataLoadResult::Success)
			OnCompletion.Execute( SlotName, UserIndex, Result, nullptr, nullptr );
		else
			LoadSaveGameFromSlot_Async( WeakWorldContext.Get( ), SlotName, UserIndex, OnCompletion );
	};

	FindMostRecentSave_Async( WorldContext, UserIndex, FLoadHeaderAsyncCallback::CreateLambda( OnFindComplete ), Filter );
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

FEnumeratedSaveGameHeader UUpliftCampaignSaveUtilities::FindMostRecentSave( const UObject *WorldContext, int32 UserIndex, const FSaveFilter &Filter )
{
	FEnumeratedHeader_Core CoreResult;
	if (Filter.IsBound( ))
	{
		auto FilterLambda = [ Filter ]( const FString &SlotName, int32 UserIndex, const USaveDataHeader *Header, ESaveDataLoadResult LoadingResult ) -> bool
		{
			const auto GameHeader = CastChecked< UUpliftCampaignSaveHeader >( Header );
			return Filter.Execute( SlotName, UserIndex, GameHeader, LoadingResult );
		};

		CoreResult = Super::FindMostRecentSave( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), FSaveFilter_Core::CreateLambda( FilterLambda ) );
	}
	else
	{
		CoreResult = Super::FindMostRecentSave( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ) );
	}

	return FEnumeratedSaveGameHeader( CoreResult );
}

void UUpliftCampaignSaveUtilities::FindMostRecentSave_Async( const UObject *WorldContext, int32 UserIndex, const FLoadHeaderAsyncCallback &OnCompletion, const FSaveFilter &Filter )
{
	check( OnCompletion.IsBound( ) );

	auto CompletionLambda = [ OnCompletion ]( const FString &SlotName, int32 UserIndex, ESaveDataLoadResult Result, const USaveDataHeader *Header ) -> void
	{
		OnCompletion.Execute( SlotName, UserIndex, Result, CastChecked< UUpliftCampaignSaveHeader >( Header, ECastCheckedType::NullAllowed ) );
	};
	const auto OnCompletion_Core = Super::FLoadHeaderAsyncCallback_Core::CreateLambda( CompletionLambda );

	if (!Filter.IsBound( ))
		return Super::FindMostRecentSave_Async( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), OnCompletion_Core );

	auto FilterLambda = [ Filter ]( const FString &SlotName, int32 UserIndex, const USaveDataHeader *Header, ESaveDataLoadResult LoadingResult ) -> bool
	{
		const auto GameHeader = CastChecked< UUpliftCampaignSaveHeader >( Header );
		return Filter.Execute( SlotName, UserIndex, GameHeader, LoadingResult );
	};

	Super::FindMostRecentSave_Async( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), OnCompletion_Core, FSaveFilter_Core::CreateLambda( FilterLambda ) );
}

FEnumeratedSaveGameHeader UUpliftCampaignSaveUtilities::FindLeastRecentSave( const UObject *WorldContext, int32 UserIndex, const FSaveFilter &Filter )
{
	FEnumeratedHeader_Core CoreResult;
	if (Filter.IsBound( ))
	{
		auto FilterLambda = [ Filter ]( const FString &SlotName, int32 UserIndex, const USaveDataHeader *Header, ESaveDataLoadResult LoadingResult ) -> bool
		{
			const auto GameHeader = CastChecked< UUpliftCampaignSaveHeader >( Header );
			return Filter.Execute( SlotName, UserIndex, GameHeader, LoadingResult );
		};

		CoreResult = Super::FindLeastRecentSave( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), FSaveFilter_Core::CreateLambda( FilterLambda ) );
	}
	else
	{
		CoreResult = Super::FindLeastRecentSave( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ) );
	}

	return FEnumeratedSaveGameHeader( CoreResult );
}

void UUpliftCampaignSaveUtilities::FindLeastRecentSave_Async( const UObject *WorldContext, int32 UserIndex, const FLoadHeaderAsyncCallback &OnCompletion, const FSaveFilter &Filter )
{
	check( OnCompletion.IsBound( ) );

	auto CompletionLambda = [ OnCompletion ]( const FString &SlotName, int32 UserIndex, ESaveDataLoadResult Result, const USaveDataHeader *Header ) -> void
	{
		OnCompletion.Execute( SlotName, UserIndex, Result, CastChecked< UUpliftCampaignSaveHeader >( Header, ECastCheckedType::NullAllowed ) );
	};
	const auto OnCompletion_Core = Super::FLoadHeaderAsyncCallback_Core::CreateLambda( CompletionLambda );

	if (!Filter.IsBound( ))
		return Super::FindLeastRecentSave_Async( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), OnCompletion_Core );

	auto FilterLambda = [ Filter ]( const FString &SlotName, int32 UserIndex, const USaveDataHeader *Header, ESaveDataLoadResult LoadingResult ) -> bool
	{
		const auto GameHeader = CastChecked< UUpliftCampaignSaveHeader >( Header );
		return Filter.Execute( SlotName, UserIndex, GameHeader, LoadingResult );
	};

	Super::FindLeastRecentSave_Async( WorldContext, UserIndex, UUpliftCampaignSaveHeader::StaticClass( ), OnCompletion_Core, FSaveFilter_Core::CreateLambda( FilterLambda ) );
}

const UUpliftCampaignSave* UUpliftCampaignSaveUtilities::CreateTravelSave( const UObject *WorldContext, const TSoftObjectPtr< const UWorld > &Destination )
{
	const auto Save = CreateAndFillSaveData( WorldContext, false, true );

	Save->WorldToLoad = Destination;

	return Save;
}

const UUpliftCampaignSave* UUpliftCampaignSaveUtilities::CreateCheckpointSave( const UObject *WorldContext )
{
	return CreateAndFillSaveData( WorldContext, false, false );
}

void UUpliftCampaignSaveUtilities::CreateCheckpointSave_Async( const UObject *WorldContext, const FCreateCheckpointComplete &OnCompletion )
{
	check( OnCompletion.IsBound( ) );

	const auto CheckpointData = CreateSaveData( WorldContext, false );

	FillAsyncSaveGameData_Async( WorldContext, CheckpointData, false, OnCompletion );
}

FString UUpliftCampaignSaveUtilities::FindBestAutoSaveSlotName( const UObject *WorldContext, int32 UserIndex )
{
	const auto AvailableSlot = Super::GetUnusedSlotName( UserIndex, AutoSavePrefix, CVar_MaxAutoSaveSlots.GetValueOnAnyThread( ) );

	if (!AvailableSlot.IsEmpty( ))
		return AvailableSlot;

	const auto AutoSaveFilter = FSaveFilter::CreateLambda( [ ]( const FString &SlotName, int32 UserIndex, const UUpliftCampaignSaveHeader *Header, ESaveDataLoadResult LoadingResult ) -> bool
	{
		return Header->SaveType == ESaveGameType::Auto;
	});
	
	const auto Oldest = FindLeastRecentSave( WorldContext, UserIndex, AutoSaveFilter );
	return Oldest.SlotName;
}

void UUpliftCampaignSaveUtilities::FindBestAutoSaveSlotName_Async( const UObject *WorldContext, int32 UserIndex, const FSaveAsyncCallback &OnCompletion )
{
	check( OnCompletion.IsBound( ) );

	TWeakObjectPtr< const UObject > WorldPtr( WorldContext );
	const auto FindComplete = FSaveAsyncCallback_Core::CreateLambda( [ WorldPtr, OnCompletion ]( const FString &SlotName, int32 UserIndex, bool Success )
	{
		if (Success)
		{
			OnCompletion.Execute( SlotName, UserIndex, true );
			return;
		}

		if (!ensureAlways( WorldPtr.IsValid( ) ))
		{
			OnCompletion.Execute( FString( ), UserIndex, false );
			return;
		}

		const auto AutoSaveFilter = FSaveFilter::CreateLambda( [ ]( const FString &SlotName, int32 UserIndex, const UUpliftCampaignSaveHeader *Header, ESaveDataLoadResult LoadingResult ) -> bool
		{
			if (Header->SaveType != ESaveGameType::Auto)
				return false; // ignore non-auto saves

			if (!SlotName.StartsWith( AutoSavePrefix ))
				return false; // ignore saves created with non-standard names

			if (!SlotName.LeftChop( AutoSavePrefix.Len( ) ).IsNumeric( ))
				return false; // ignore saves that don't have a numeric suffix

			return true;
		});
	
		const auto FoundOldest = FLoadHeaderAsyncCallback::CreateLambda( [ OnCompletion ]( const FString &SlotName, int32 UserIndex, ESaveDataLoadResult Result, const UUpliftCampaignSaveHeader *Header )
		{
			if (Header == nullptr)
				OnCompletion.Execute( FString( ), UserIndex, false );
			else
				OnCompletion.Execute( SlotName, UserIndex, true );
		});

		FindLeastRecentSave_Async( WorldPtr.Get( ), UserIndex, FoundOldest, AutoSaveFilter );
	});

	Super::GetUnusedSlotName_Async( WorldContext, UserIndex, FindComplete, AutoSavePrefix, CVar_MaxAutoSaveSlots.GetValueOnAnyThread( ) );
}

bool UUpliftCampaignSaveUtilities::AutoSave( const UObject *WorldContext, int32 UserIndex, const FString &DisplayNameOverride )
{
	if (!ensureAlways( !AnyAsyncSaveTasksPending( WorldContext )))
		return false;
	
	const auto SlotName = FindBestAutoSaveSlotName( WorldContext, UserIndex );

	return SaveToSlot( WorldContext, SlotName, UserIndex, ESaveGameType::Auto, DisplayNameOverride );
}

void UUpliftCampaignSaveUtilities::AutoSave_Async( const UObject *WorldContext, int32 UserIndex, const FString &DisplayNameOverride, const FSaveAsyncCallback &OnCompletion )
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

	struct FAutoSaveTask : public FSaveDataTask
	{
		FAutoSaveTask( int UI, const FString &DN ) : FSaveDataTask( UI ), DisplayName( DN ) { }

		// Begin the multiple async task required for creating an async auto save - creating the save and determining the name
		void Branch(const UObject *WorldContext) override
		{
			Context = WorldContext;
			
			const auto FillComplete = FCreateCheckpointComplete::CreateLambda( [ this ]( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, bool Success )
			{
				if (Success)
				{
					SaveData = TStrongObjectPtr( CheckpointData );
				}
				else
				{
					bResult = false;
				}

				++CompletionCount;
			});
			FillAsyncSaveGameData_Async( WorldContext, CreateSaveData( WorldContext ), true, FillComplete );

			const auto FindSlotName = FSaveAsyncCallback::CreateLambda( [ this ](const FString &FoundSlotName, int32 /*UserIndex*/, bool Success)
			{
				if (Success)
					SlotName = FoundSlotName;
				else
					bResult = false;

				++CompletionCount;
			});
			FindBestAutoSaveSlotName_Async( WorldContext, UserIndex, FindSlotName );
		}

		void DoWork( void )
		{
			TRACE_CPUPROFILER_EVENT_SCOPE( TEXT( "FAutoSaveTask::DoWork" ) )

			while (CompletionCount < 2) // wait until both async tasks are completed
				FPlatformProcess::Sleep( 0.1f );

			if (!bResult)
				return; // something went wrong so quit

			if (DisplayName.IsEmpty( ))
				DisplayName = SlotName.Replace( TEXT( "_" ), TEXT( " " ) );

			FGCScopeGuard GCGuard;

			Header = CreateSaveGameHeader( SaveData.Get( ), ESaveGameType::Auto, DisplayName );
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
		TStrongObjectPtr< const UUpliftCampaignSave > SaveData;

		// The header metadata about the save
		UUpliftCampaignSaveHeader *Header = nullptr;
		
		// The user facing name that should be used for this save
		FString DisplayName;
		
		// The slot that the save data should be written to
		FString SlotName;

		// The ultimate outcome of this async task
		bool bResult = true;

		// The context in which the operation is running
		const UObject *Context = nullptr;

		// Counter to track the completion of subordinate async tasks
		// The completion delegates are run on the game thread, so there's no potential concurrency issue
		// This async task only reads, and it doesn't really matter if the read misses with either increment
		int CompletionCount = 0;
		
	} NewTask( UserIndex, DisplayNameOverride );

	const auto AsyncComplete = FAsyncTaskComplete< FAutoSaveTask >::CreateLambda( [ OnCompletion ]( const UObject *WorldContext, const FAutoSaveTask &Task )
	{
		OnCompletion.ExecuteIfBound( Task.SlotName, Task.UserIndex, Task.bResult );
	});

	if (!StartAsyncSaveTask( WorldContext, MoveTemp( NewTask ), "Auto Save", AsyncComplete ))
		OnCompletion.ExecuteIfBound( FString( ), UserIndex, false );
}

bool UUpliftCampaignSaveUtilities::QuickSave( const UObject *WorldContext, int32 UserIndex )
{
	if (!IsManualSavingAllowed( WorldContext ))
		return false;
	
	return SaveToSlot( WorldContext, GetQuickSaveSlotName( ), UserIndex, ESaveGameType::Quick, GetQuickSaveDisplayName( ) );
}

void UUpliftCampaignSaveUtilities::QuickSave_Async( const UObject *WorldContext, int32 UserIndex, const FSaveAsyncCallback &OnCompletion )
{
	const auto SlotName = GetQuickSaveSlotName( );
	
	if (!IsManualSavingAllowed( WorldContext ))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}
	
	SaveToSlot_Async( WorldContext, SlotName, UserIndex, ESaveGameType::Quick, GetQuickSaveDisplayName( ), OnCompletion );
}

bool UUpliftCampaignSaveUtilities::SaveToPath( const UObject *WorldContext, const FString &PathName, ESaveGameType SaveType, FString DisplayNameOverride )
{
	check( IsInGameThread( ) );

	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
		return false; // Ignore saving a developer save

	const auto SaveData = CreateAndFillSaveData( WorldContext, true, false );
	if (!ensureAlways( SaveData != nullptr ))
		return false;

	if (DisplayNameOverride.IsEmpty( ))
	{
		int Index = INDEX_NONE;
		PathName.FindLastChar( '/', Index );

		DisplayNameOverride = PathName.Right( PathName.Len( ) - Index - 1 );

		DisplayNameOverride = DisplayNameOverride.Replace( TEXT( "_" ), TEXT( " " ) );
	}

	const auto Header = CreateSaveGameHeader( SaveData, SaveType, DisplayNameOverride );
	if (!ensureAlways( Header != nullptr ))
		return false;

	return Super::SaveDataToPath( WorldContext, Header, SaveData, PathName );
}

void UUpliftCampaignSaveUtilities::SaveToPath_Async( const UObject *WorldContext, const FString &PathName, ESaveGameType SaveType, FString DisplayNameOverride, const FSaveAsyncCallback &OnCompletion )
{
	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
	{
		OnCompletion.ExecuteIfBound( PathName, -1, false );
		return; // Ignore saving a developer save
	}

	const auto SaveData = CreateSaveData( WorldContext );
	if (!ensureAlways( SaveData != nullptr ))
	{
		OnCompletion.ExecuteIfBound( PathName, -1, false );
		return;
	}

	if (DisplayNameOverride.IsEmpty( ))
	{
		int Index = INDEX_NONE;
		PathName.FindLastChar( '/', Index );

		DisplayNameOverride = PathName.Right( PathName.Len( ) - Index - 1 );

		DisplayNameOverride = DisplayNameOverride.Replace( TEXT( "_" ), TEXT( " " ) );
	}

	const auto AsyncFillComplete = FCreateCheckpointComplete::CreateLambda( [ PathName, SaveType, DisplayNameOverride, OnCompletion ]( const UObject *WorldContext, const UUpliftCampaignSave* CheckpointData, bool Success )
	{
		if (!Success)
		{
			OnCompletion.ExecuteIfBound( PathName, -1, false );
			return;
		}

		const UUpliftCampaignSaveHeader* Header = CreateSaveGameHeader( CheckpointData, SaveType, DisplayNameOverride );
		if (!ensureAlways( Header != nullptr ))
		{
			OnCompletion.ExecuteIfBound( PathName, -1, false );
			return;
		}

		SaveDataToPath_Async( WorldContext, Header, CheckpointData, PathName, OnCompletion );
	});

	FillAsyncSaveGameData_Async( WorldContext, SaveData, true, AsyncFillComplete );
}

void UUpliftCampaignSaveUtilities::SaveCheckpointToPath( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, const FString &PathName, ESaveGameType SaveType, FString DisplayNameOverride )
{
	check( IsInGameThread( ) );

	if (!CVar_AllowDeveloperSaves.GetValueOnAnyThread( ) && (SaveType == ESaveGameType::Developer))
		return; // Ignore saving a developer save
	
	if (!ensureAlways( CheckpointData != nullptr ))
		return;
	if (!ensureAlways( CheckpointData->bCreationComplete == true ))
		return;

	if (DisplayNameOverride.IsEmpty( ))
	{
		int Index = INDEX_NONE;
		PathName.FindLastChar( '/', Index );

		DisplayNameOverride = PathName.Right( PathName.Len( ) - Index - 1 );

		DisplayNameOverride = DisplayNameOverride.Replace( TEXT( "_" ), TEXT( " " ) );
	}

	const auto SaveGameData = CastChecked< UUpliftCampaignSave >( StaticDuplicateObject( CheckpointData, GetTransientPackage( ) ) );

	if (!ensureAlways( FillCheckpointData( WorldContext, SaveGameData ) ))
		return;
	SaveGameData->bCreationComplete = true;

	const auto Header = CreateSaveGameHeader( SaveGameData, SaveType, DisplayNameOverride );
	if (!ensureAlways( Header != nullptr ))
		return;

	const auto SaveDataResult = Super::SaveDataToPath( WorldContext, Header, SaveGameData, PathName );
	ensureAlways( SaveDataResult );
}

void UUpliftCampaignSaveUtilities::SaveCheckpointToPath_Async( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, const FString &PathName, ESaveGameType SaveType, FString DisplayNameOverride, const FSaveAsyncCallback &OnCompletion )
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

	if (DisplayNameOverride.IsEmpty( ))
	{
		int Index = INDEX_NONE;
		PathName.FindLastChar( '/', Index );

		DisplayNameOverride = PathName.Right( PathName.Len( ) - Index - 1 );

		DisplayNameOverride = DisplayNameOverride.Replace( TEXT( "_" ), TEXT( " " ) );
	}

	struct FSaveCheckpointTask : public FSaveDataTask
	{
		FSaveCheckpointTask( const FString &PN, ESaveGameType ST, const FString &DN, const UUpliftCampaignSave *CP ) : FSaveDataTask( -1 ), PathName( PN ), DisplayName( DN ), SaveType( ST ), CheckpointData( CP ) { }

		void Branch(const UObject *WorldContext) override
		{
			Context = WorldContext;
		}

		void DoWork( )
		{
			SaveGameData = CastChecked< UUpliftCampaignSave >( StaticDuplicateObject( CheckpointData.Get( ), GetTransientPackage( ) ) );
			if (SaveGameData == nullptr)
				return;

			if (!FillCheckpointData( Context, SaveGameData ))
				return;
			SaveGameData->bCreationComplete = true;

			Header = CreateSaveGameHeader( SaveGameData, SaveType, DisplayName );
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
		FString DisplayName;

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

	} NewTask( PathName, SaveType, DisplayNameOverride, CheckpointData );

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
bool UUpliftCampaignSaveUtilities::DeveloperSave( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, FString DisplayNameOverride )
{
	if (!ensureAlways( !SlotName.IsEmpty( ) ))
		return false;

	if (DisplayNameOverride.IsEmpty( ))
		DisplayNameOverride = SlotName.Replace( TEXT( "_" ), TEXT( " " ) );
	
	return SaveToSlot( WorldContext, DevSavePrefix + SlotName, UserIndex, ESaveGameType::Developer, DisplayNameOverride );
}

void UUpliftCampaignSaveUtilities::DeveloperSave_Async( const UObject *WorldContext, const FString &SlotName, int32 UserIndex, FString DisplayNameOverride, const FSaveAsyncCallback &OnCompletion )
{
	if (!ensureAlways( !SlotName.IsEmpty( ) ))
	{
		OnCompletion.ExecuteIfBound( SlotName, UserIndex, false );
		return;
	}
	
	if (DisplayNameOverride.IsEmpty( ))
		DisplayNameOverride = SlotName.Replace( TEXT( "_" ), TEXT( " " ) );

	SaveToSlot_Async( WorldContext, DevSavePrefix + SlotName, UserIndex, ESaveGameType::Developer, DisplayNameOverride, OnCompletion );
}
#endif