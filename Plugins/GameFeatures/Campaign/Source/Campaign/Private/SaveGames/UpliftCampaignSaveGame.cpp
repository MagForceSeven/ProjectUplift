// Copyright Russell Aasland. All Rights Reserved.

#include "SaveGames/UpliftCampaignSaveGame.h"

#include "SaveGames/UpliftCampaignSaveVersion.h"
#include "SaveGames/UpliftCampaignSaveUtilities.h"
#include "SaveGames/UpliftCampaignSaveSubsystem.h"

#include "DataStoreActor.h"
#include "PersistenceComponent.h"
#include "PersistentActorArchiver.h"

// Model View View Model
#include "MVVMViewModelBase.h"

// Engine
#include "Engine/AssetManager.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftCampaignSaveGame)

UUpliftCampaignSave* UUpliftCampaignSaveUtilities::CreateSaveData( const UObject *WorldContext, bool bIsTravelSave )
{
	check( IsInGameThread( ) );

	if (!ensureAlways( WorldContext != nullptr ))
		return nullptr;

	auto SaveGame = NewObject< UUpliftCampaignSave >( GetTransientPackage( ) );

	auto CurrentWorld = GEngine->GetWorldFromContextObject( WorldContext, EGetWorldErrorMode::Assert );
	if (CurrentWorld == nullptr)
		return nullptr;

	SaveGame->FillCoreData( WorldContext, (uint32)EUpliftCampaignSaveVersion::Build_Latest );

	FSoftObjectPath Path( CurrentWorld );
#if WITH_EDITOR
	const auto AssetPath = Path.GetAssetPathString( );
	if (AssetPath.Contains( PLAYWORLD_PACKAGE_PREFIX ))
		Path = FSoftObjectPath( FTopLevelAssetPath( UWorld::RemovePIEPrefix( AssetPath ) ), Path.GetSubPathString( ) );
#endif

	SaveGame->WorldToLoad = Path;
	SaveGame->bTravelSave = bIsTravelSave;

	FMemoryWriter MemoryWriter( SaveGame->PersistentActorData );
	FPersistentActorWriter Archiver( MemoryWriter );
	SaveGame->ConfigureArchiveVersions( Archiver );

	static const auto SaveSettings = GetDefault< UUpliftCampaignSaveSettings >( );

	if (bIsTravelSave)
	{
		Archiver.ObjectFilter = [ /*Settings = SaveSettings,*/ ]( const UObject *Object ) -> bool
		{
			// All of these engine classes are possible to change type on a level transition, so we shouldn't include them in this type of save
			if (Object->IsA< AGameModeBase >( ))
				return false;
			if (Object->IsA< AGameStateBase >( ))
				return false;
			if (Object->IsA< APlayerController >( ))
				return false;

			// don't include persistent actors that are part of the world that we are leaving
			if (const auto Actor = Cast< AActor >( Object ))
			{
				if (const auto PersistenceComponent = Actor->GetComponentByClass< UPersistenceComponent >( ))
				{
					if (!PersistenceComponent->WasSpawned( ))
						return false;
				}
			}
		
			return true;
		};
	}

	Archiver.ComponentFilter = [ Settings = SaveSettings/*, bIsTravelSave*/ ]( const UActorComponent *Component ) -> bool
	{
		if (Component->GetOwner( )->IsA< ADataStoreActor >( ))
			return true; // keep all components on DataStoreActors. maybe add an exclusion list later

		for (const auto &ComponentType : Settings->SavedComponentTypes)
		{
			const auto ComponentClass = ComponentType.Get( );
			if (ComponentClass == nullptr)
				continue; // if the class isn't loaded, there can't be an instance of it anywhere, so it doesn't matter

			if (Component->IsA( ComponentClass ))
				return true;
		}

		return false;
	};

	Archiver.SubObjectFilter = [ Settings = SaveSettings/*, bIsTravelSave*/ ]( const UObject *SubObject ) -> bool
	{
		// Never save any references to view models, they are always recreated on load
		if (SubObject->IsA< UMVVMViewModelBase >( ))
			return false;

		for (const auto &IgnoredTypes : Settings->IgnoredSubobjectTypes)
		{
			const auto Type = IgnoredTypes.Get( );
			if (Type == nullptr)
				continue; // if the class isn't loaded, there can't be an instance of it anywhere so it doesn't matter

			if (SubObject->IsA( Type ))
				return false;
		}

		return true;
	};

	Archiver.Archive( WorldContext );

	SaveGame->PersistentActorClasses.Reserve( Archiver.SavedObjectClasses.Num( ) );
	Algo::Transform( Archiver.SavedObjectClasses, SaveGame->PersistentActorClasses, [ ]( const TSoftClassPtr< UObject > &C ) { return C.ToSoftObjectPath( ); } );

	return SaveGame;
}

bool UUpliftCampaignSaveUtilities::FillAsyncSaveGameData( UUpliftCampaignSave *SaveGame )
{
	if (!ensureAlways( SaveGame != nullptr ))
		return false;
	if (!ensureAlways( !SaveGame->bCreationComplete ))
		return false;

	// TODO: Fill in the save game here which can be gathered asynchronously

	SaveGame->bCreationComplete = true;

	return true;
}

void UUpliftCampaignSaveUtilities::FillAsyncSaveGameData_Async( const UObject *WorldContext, UUpliftCampaignSave *SaveGame, bool bIncludeCheckpoints, const FCreateCheckpointComplete &OnCompletion )
{
	check( SaveGame != nullptr );
	check( OnCompletion.IsBound( ) );

	// Async task for filling in the data of a save game with data
	struct FFillSaveData : public FSaveDataTask
	{
		FFillSaveData( UUpliftCampaignSave *S, bool CP ) : SaveGame( S )
		{
		}

		void Branch(const UObject *WorldContext) override
		{
			Context = WorldContext;
		}

		void DoWork( )
		{
			Result = FillAsyncSaveGameData( SaveGame.Get( ) );
		}

		// The save game data that should be filled in
		TStrongObjectPtr< UUpliftCampaignSave > SaveGame;

		// The world that we're creating the save data from
		const UObject *Context = nullptr;

		// Was the fill was completed successfully?
		bool Result = false;

	} NewTask( SaveGame, bIncludeCheckpoints );

	const auto OnFillComplete = FAsyncTaskComplete< FFillSaveData >::CreateLambda( [ OnCompletion ]( const UObject *WorldContext, const FFillSaveData &Task )
	{
		OnCompletion.Execute( WorldContext, Task.SaveGame.Get( ), Task.Result );
	});

	if (!StartAsyncSaveTask( WorldContext, MoveTemp( NewTask ), "Fill Async Save Data", OnFillComplete ))
		OnCompletion.Execute( WorldContext, SaveGame, false );
}

bool UUpliftCampaignSaveUtilities::FillCheckpointData( const UObject *WorldContext, UUpliftCampaignSave *SaveGame )
{
	const auto Subsystem = UUpliftCampaignSaveSubsystem::Get( WorldContext );
	check( Subsystem != nullptr );

	if (Subsystem->TacticalStartCheckpoint != nullptr)
		SaveGame->SerializeToBytes( Subsystem->TacticalStartCheckpoint, SaveGame->TacticalCheckpointBytes );

	return true;
}

void UUpliftCampaignSaveUtilities::FillCheckpointData_Async( const UObject *WorldContext, UUpliftCampaignSave *SaveGame, const FCreateCheckpointComplete &OnCompletion )
{
	if (!ensureAlways( SaveGame != nullptr ))
		return;
	if (!ensureAlways( OnCompletion.IsBound( ) ))
		return;

	// Async task for filling in the data of a save game with data
	struct FFillCheckpointData : public FSaveDataTask
	{
		FFillCheckpointData( UUpliftCampaignSave *S ) : SaveGame( S )
		{
		}

		void Branch(const UObject *WorldContext) override
		{
			Context = WorldContext;
		}

		void DoWork( )
		{
			Result = FillCheckpointData( Context, SaveGame.Get( ) );
		}

		// The save game data that should be filled in
		TStrongObjectPtr< UUpliftCampaignSave > SaveGame;

		// The world that we're creating the save data from
		const UObject *Context = nullptr;

		// Was the fill was completed successfully?
		bool Result = false;

	} NewTask( SaveGame );

	const auto OnFillComplete = FAsyncTaskComplete< FFillCheckpointData >::CreateLambda( [ OnCompletion ]( const UObject *InWorldContext, const FFillCheckpointData &Task )
	{
		OnCompletion.Execute( InWorldContext, Task.SaveGame.Get( ), Task.Result );
	});

	if (!StartAsyncSaveTask( WorldContext, MoveTemp( NewTask ), "Fill Async Checkpoint Data", OnFillComplete ))
		OnCompletion.Execute( WorldContext, SaveGame, false );
}

UUpliftCampaignSave* UUpliftCampaignSaveUtilities::CreateAndFillSaveData( const UObject *WorldContext, bool IncludeCheckpointData, bool bTravelSave )
{
	const auto SaveData = CreateSaveData( WorldContext, bTravelSave );

	if (IncludeCheckpointData && !FillCheckpointData( WorldContext, SaveData ))
		return nullptr;

	if (!FillAsyncSaveGameData( SaveData ))
		return nullptr;

	return SaveData;
}

static TAutoConsoleVariable< int > CVar_MinAllowedSaveVersion( TEXT( "Uplift.SaveGames.SetMinAllowedSaveVersion" ),
	(int)EUpliftCampaignSaveVersion::Build_Minimum, TEXT( "Change the minimum supported version for loading save games" ), ECVF_Cheat );
static TAutoConsoleVariable< int > CVar_MaxAllowedSaveVersion( TEXT( "Uplift.SaveGames.SetMaxAllowedSaveVersion" ),
	(int)EUpliftCampaignSaveVersion::Build_Latest, TEXT( "Change the maximum supported version for loading save games" ), ECVF_Cheat );

bool UUpliftCampaignSave::IsCompatible( uint32 InVersion, uint32 InChangelist ) const
{
#if !SF_SAVES_ALLOW_DEV
	// If this version isn't RTM, we can't load it
	if ((InVersion & (int32)EUpliftCampaignSaveVersion::RTM) == 0)
		return false;
#endif

	// Convert both the version and minimum version to build-agnostic values
	const auto Version = UpliftCampaignSave_StripRTM( (EUpliftCampaignSaveVersion)InVersion );

	const auto MinimumCompatibleVersion = (EUpliftCampaignSaveVersion)CVar_MinAllowedSaveVersion.GetValueOnAnyThread( );
	const auto BuildMinimum = UpliftCampaignSave_StripRTM( MinimumCompatibleVersion );

	// Below the minimum allowed for this build
	if (Version < BuildMinimum)
		return false;

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	const auto MaximumCompatibleVersion = (EUpliftCampaignSaveVersion)CVar_MaxAllowedSaveVersion.GetValueOnAnyThread( );
	const auto BuildMaximum = UpliftCampaignSave_StripRTM( MaximumCompatibleVersion );
	if (Version > BuildMaximum)
		return false;
#endif

	// Above the maximum known to this build
	if (Version > EUpliftCampaignSaveVersion::Latest)
		return false;

	// Is the changelist below the minimum build available
	if (InChangelist < Minimum_Allowed_CL)
		return false;

	return true;
}

bool UUpliftCampaignSave::ApplySaveData( const UObject *WorldContext ) const
{
	check( IsInGameThread( ) );

	FMemoryReader MemoryReader( PersistentActorData );
	FPersistentActorReader Archiver( MemoryReader );
	ConfigureArchiveVersions( Archiver );

	Archiver.Archive( WorldContext );

	if (TacticalCheckpointBytes.Num( ) > 0)
	{
		const auto CheckpointData = NewObject< UUpliftCampaignSave >( GetTransientPackage( ) );
		SerializeFromBytes( CheckpointData, TacticalCheckpointBytes );

		const auto Subsystem = UUpliftCampaignSaveSubsystem::Get( WorldContext );
		check( Subsystem != nullptr );

		Subsystem->TacticalStartCheckpoint = CheckpointData;
	}

	return true;
}

TSharedPtr< FStreamableHandle > UUpliftCampaignSave::LoadAssets( ) const
{
	return UAssetManager::Get( ).GetStreamableManager(  ).RequestAsyncLoad( PersistentActorClasses );
}
