// Copyright Russell Aasland. All Rights Reserved.

#include "GameWorld/CampaignGameMode.h"

#include "CampaignBundles.h"
#include "Campaign/CampaignGameInstance.h"
#include "GameWorld/CampaignGameState.h"
#include "GameWorld/CampaignPlayerController.h"
#include "GameWorld/CampaignPlayerState.h"
#include "GameWorld/CampaignHUD.h"
#include "GameWorld/CampaignPawn.h"
#include "Campaign/CampaignModeScopeCollection.h"

#include "Settings/CampaignSettings.h"
#include "Settings/CampaignPIESettings.h"
#include "Strategy/NewGameSettings.h"
#include "Tactical/SimulationSettings.h"

#include "SaveGames/UpliftCampaignSaveGame.h"
#include "SaveGames/UpliftCampaignSaveSubsystem.h"
#include "SaveGames/UpliftCampaignSaveUtilities.h"
#include "GameFeatures/UpliftGameFeatureData.h"
#include "GameFeatures/UpliftGameFeatureProjectPolicy.h"

#include "DataDefinitions/CampaignDifficultyDefinition.h"
#include "DataDefinitions/Extensions/CampaignDifficultyExtension.h"
#include "DataStoreActors/Campaign.h"

#include "PersistentDataStore.h"
#include "DataStoreUtilities.h"
#include "DataStoreVisualizer.h"

#include "GameFeatures/FeatureContentManager.h"
#include "DataDefinitions/DataDefinitionLibrary.h"
#include "Perf/SplatTaskManager.h"

// Engine
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CampaignGameMode)

ACampaignGameMode::ACampaignGameMode( )
{
	GameStateClass = ACampaignGameState::StaticClass( );
	PlayerControllerClass = ACampaignPlayerController::StaticClass( );
	PlayerStateClass = ACampaignPlayerState::StaticClass( );
	DefaultPawnClass = ACampaignPawn::StaticClass( );
	HUDClass = ACampaignHUD::StaticClass( );

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ACampaignGameMode::HandleMatchIsWaitingToStart( )
{
	const auto CampaignInstance = UCampaignGameInstance::Get( this ); check( CampaignInstance != nullptr );
	const auto FeaturesManager = UFeatureContentManager::GetSubsystem( this ); check( FeaturesManager != nullptr );
	const auto SaveGames = UUpliftCampaignSaveSubsystem::GetSubsystem( this ); check( SaveGames != nullptr );

	auto InitializationSplat = [ this, CampaignInstance, FeaturesManager, SaveGames ]( ) -> ESplatTaskResult
	{
		switch (InitStage)
		{
			case ECampaignInitializationStage::Init:
				UUpliftGameFeatureProjectPolicy::AdditionalPreloadBundles.AddUnique( CampaignBundles::Campaign );
	#if !UE_BUILD_SHIPPING
				if (CampaignInstance->IsQuickplay( ) && (SaveGames->SaveGame == nullptr))
				{
					FeaturesManager->EnableDeveloperPIEFeatures( GetWorld( ) );

					const auto CampaignFeatures = FeaturesManager->GetKnownFeatures( UUpliftGameFeatureData::ContentType_Campaign );
					FeaturesManager->EnableFeatures( TSet( CampaignFeatures ) );
				}
	#endif
				if (SaveGames->SaveGame != nullptr)
				{
					FeaturesManager->EnableFeatures( TSet( SaveGames->SaveGame->ContentFeatures ) );
					StreamHandle = SaveGames->SaveGame->LoadAssets( );
				}
				else if (CampaignInstance->NewGameSettings != nullptr)
				{
					FeaturesManager->EnableFeatures( TSet( CampaignInstance->NewGameSettings->Packages ) );
				}
				else if (CampaignInstance->SimulationSettings != nullptr)
				{
					FeaturesManager->EnableFeatures( TSet( CampaignInstance->SimulationSettings->Packages ) );
				}

				InitStage = ECampaignInitializationStage::WaitOnFeatures;
				// intentional fallthrough

			case ECampaignInitializationStage::WaitOnFeatures:
				if (!FeaturesManager->AreEnabledFeaturesActive( ))
					break;
				if (StreamHandle.IsValid( ) && !StreamHandle->HasLoadCompleted( ))
					break;

				InitStage = ECampaignInitializationStage::InitGameData;
				// intentional fallthrough

			case ECampaignInitializationStage::InitGameData:
				InitGameModelData( );
				StreamHandle.Reset( ); // save game would have created the hard references to keep anything here alive
				InitStage = ECampaignInitializationStage::ToggleBundles;
				// intentional fallthrough

			case ECampaignInitializationStage::ToggleBundles:
			{
				const auto Library = UDataDefinitionLibrary::GetInstance( );

				GameModeBundleAssets = GatherAssetsForModeBundles( );

				if (!GameModeBundleAssets.IsEmpty( ))
					StreamHandle = Library->ChangeBundleStateForPrimaryAssetsAndDependencies( GameModeBundleAssets, GameModeBundles, { } );

				InitStage = ECampaignInitializationStage::WaitOnBundles;
				// intentional fallthrough
			}
				
			case ECampaignInitializationStage::WaitOnBundles:
				if (StreamHandle.IsValid( ) && !StreamHandle->HasLoadCompleted( ))
					break;

				StreamHandle.Reset( ); // The asset manager is keeping these alive with its own reference
				InitStage = ECampaignInitializationStage::Complete;
				// intentional fallthrough

			case ECampaignInitializationStage::Complete:
				SetActorTickEnabled( true );
				Super::HandleMatchIsWaitingToStart( );
				return ESplatTaskResult::Complete;
		}

		return ESplatTaskResult::Yield;
	};

	const auto TaskManager = USplatTaskManager::GetSubsystem( this );
	TaskManager->StartTask( MoveTemp( InitializationSplat ) );
}

bool ACampaignGameMode::ReadyToStartMatch_Implementation( )
{
	if (InitStage != ECampaignInitializationStage::Complete)
		return false;

	return Super::ReadyToStartMatch_Implementation( );
}

void ACampaignGameMode::BeginPlay( )
{
	Super::BeginPlay( );

#if WITH_EDITOR
	FEditorDelegates::PrePIEEnded.AddWeakLambda( this, [ ]( bool bIsSimulating ) -> void
	{
		UUpliftGameFeatureProjectPolicy::AdditionalPreloadBundles.Remove( CampaignBundles::Campaign );
	} );
#endif

	// Wait until BeginPlay has been dispatched to all the actors in the world before triggering the gameplay for this world
	GetWorld( )->GetOnBeginPlayEvent( ).AddWeakLambda( this, [ this ]( bool bBegun ) -> void
	{
		GameModeReady( );

		GetWorld( )->GetOnBeginPlayEvent( ).RemoveAll( this );
	} );
}

void ACampaignGameMode::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
#if WITH_EDITOR
	FEditorDelegates::PrePIEEnded.RemoveAll( this );
#endif

	const auto SaveGames = UUpliftCampaignSaveSubsystem::GetSubsystem( this );
	SaveGames->bSaveWasLoaded = false;
	SaveGames->bSaveWasLevelTransition = false;

	Super::EndPlay( EndPlayReason );
}

void ACampaignGameMode::ReturnToShell( const UObject *WorldContext )
{
	const auto Settings = GetDefault< UCampaignSettings >( );
	const auto ShellPath = Settings->ShellLevel.GetLongPackageFName( );

	UUpliftGameFeatureProjectPolicy::AdditionalPreloadBundles.Remove( CampaignBundles::Campaign );

	UGameplayStatics::OpenLevel( WorldContext, ShellPath );
}

void ACampaignGameMode::TransitionToNewGameplayLevel( const UObject *WorldContext, const TSoftObjectPtr< const UWorld > &NewLevel )
{
	const auto GameMode = CastChecked< ACampaignGameMode >( UGameplayStatics::GetGameMode( WorldContext ) );

	GameMode->PreTransitionOutOfMode( );

	const auto TransitionSave = UUpliftCampaignSaveUtilities::CreateTravelSave( WorldContext, NewLevel );

	UUpliftCampaignSaveUtilities::LoadCheckpointSave( WorldContext, TransitionSave );
}

void ACampaignGameMode::InitGameModelData( void )
{
	InitializeCommon( );
	
	const auto SaveGames = UUpliftCampaignSaveSubsystem::GetSubsystem( this );
	check( SaveGames != nullptr );

	if (SaveGames->SaveGame != SaveGames->TacticalStartCheckpoint)
		SaveGames->TacticalStartCheckpoint = nullptr; // if we didn't load the checkpoint, we should clear it

	// if there is a save game, we apply that
	if (SaveGames->SaveGame != nullptr)
	{
		SaveGames->SaveGame->ApplySaveData( this );

		SaveGames->bSaveWasLoaded = true;
		SaveGames->bSaveWasLevelTransition = SaveGames->SaveGame->bTravelSave;

		SaveGames->SaveGame = nullptr;

		if (SaveGames->bSaveWasLevelTransition)
			TransitionIntoMode( );

		return;
	}

	InitializeForNewGame( );

	const auto CampaignInstance = UCampaignGameInstance::Get( this );
	check( CampaignInstance != nullptr );

#if !UE_BUILD_SHIPPING
	if (CampaignInstance->IsQuickplay( ))
		InitializeForQuickPlay( );
#endif

	CampaignInstance->NewGameSettings = nullptr;
	CampaignInstance->SimulationSettings = nullptr;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ACampaignGameMode::InitializeForNewGame( void )
{
	const auto DataStore = UPersistentDataStore::GetSubsystem( this );
	check( DataStore != nullptr );

	const auto CampaignInstance = UCampaignGameInstance::Get( this );
	check( CampaignInstance != nullptr );

	const auto CampaignState = DataStore->SpawnSingleton< ADS_Campaign >( );

	if (CampaignInstance->NewGameSettings != nullptr)
	{
		CampaignState->CampaignFlags = CampaignInstance->NewGameSettings->CampaignFlags;

		if (ensureAlways( CampaignInstance->NewGameSettings->Difficulty != nullptr ))
			CampaignState->Difficulty = CampaignInstance->NewGameSettings->Difficulty->FindExtensionByClass< UCampaignDifficultyExtension >( );
	}
	else if (CampaignInstance->SimulationSettings != nullptr)
	{
		CampaignState->CampaignFlags = CampaignInstance->SimulationSettings->CampaignFlags;

		if (ensureAlways( CampaignInstance->SimulationSettings->Difficulty != nullptr ))
			CampaignState->Difficulty = CampaignInstance->SimulationSettings->Difficulty->FindExtensionByClass< UCampaignDifficultyExtension >( );
	}
}

void ACampaignGameMode::InitializeCommon( void )
{
}

void ACampaignGameMode::InitializeForQuickPlay( void )
{
	const auto CampaignState = ADS_Campaign::GetSingleton( this );

	const auto PIESettings = GetDefault< UCampaignPIESettings >( );

	if (PIESettings->Difficulty != nullptr)
		CampaignState->Difficulty = PIESettings->Difficulty->FindExtensionByClass< UCampaignDifficultyExtension >( );
	else if (const auto DefaultDifficulty = UCampaignDifficultyDefinition::FindDefaultDifficulty( ))
		CampaignState->Difficulty = DefaultDifficulty->FindExtensionByClass< UCampaignDifficultyExtension >( );
	else
		ensureAlwaysMsgf( false, TEXT( "Failed to configure Difficulty for QuickPlay." ) );
}

void ACampaignGameMode::TransitionIntoMode( void )
{
}

void ACampaignGameMode::GameModeReady( void )
{
#if !UE_BUILD_SHIPPING
	const auto CampaignState = ADS_Campaign::GetSingleton( this );
	if (CampaignState->GetDifficulty( ) == nullptr)
	{
		ensureAlwaysMsgf( false, TEXT( "Failed to configure Campaign State with proper difficulty data. Ending Session." ) );

		const auto PlayerController = UGameplayStatics::GetPlayerController( this, 0 );
		PlayerController->ConsoleCommand( "quit" );

		return;
	}
#endif
}

void ACampaignGameMode::PreTransitionOutOfMode( void )
{
	const auto World = GetWorld( );

	// Data Store Actor Visualizers are no longer needed and should not persist
	// If the new game mode supports/requires visualizers, they will be created in that mode with appropriate types
	for (const auto Visualizer : TObjectRange< UDataStoreVisualizer >( ))
	{
		if (Visualizer->GetWorld( ) != World)
			continue;

		if (!IsValid( Visualizer->GetOwner( ) ))
			continue;

		// Disassociate them from any active data store actor to prevent saved references
		if (const auto AssociatedDataStore = UDataStoreUtilities::GetDataStoreActor( Visualizer ))
			UDataStoreUtilities::DisassociateVisualizer( AssociatedDataStore );

		Visualizer->GetOwner( )->Destroy( );
	}

	// All the members of the scope collection for this game mode are local and should be destroyed before they
	// contribute to the travel save
	if (const auto ModeScopeCollection = ACampaignModeScopeCollection::GetSingleton( World ))
	{
		for (const auto &Actor : ModeScopeCollection->GetCollectionMembers( ))
		{
			if (IsValid( Actor ))
				Actor->Destroy( );
		}
	}

	// Release the hold on the bundles from this game mode
	const auto Library = UDataDefinitionLibrary::GetInstance( );
	Library->ChangeBundleStateForPrimaryAssetsAndDependencies( GameModeBundleAssets, { }, GameModeBundles );
}

TArray< FPrimaryAssetId > ACampaignGameMode::GatherAssetsForModeBundles( void ) const
{
	const auto FeaturesManager = UFeatureContentManager::GetSubsystem( this );
	check( FeaturesManager != nullptr );

	return FeaturesManager->GetEnabledFeatureIDs( );
}

#if !UE_BUILD_SHIPPING
void ACampaignGameMode::DEBUG_AddGameModeAsset( const UDataDefinition *Definition )
{
	DEBUG_AddGameModeAsset( Definition->GetPrimaryAssetId( ) );
}

void ACampaignGameMode::DEBUG_AddGameModeAsset( const FPrimaryAssetId &AssetID )
{
	if (GameModeBundleAssets.Contains( AssetID ))
		return;

	GameModeBundleAssets.Push( AssetID );

	const auto Library = UDataDefinitionLibrary::GetInstance( );

	const auto Handle = Library->ChangeBundleStateForPrimaryAssetsAndDependencies( { AssetID }, GameModeBundles, { } );

	if (!Handle.IsValid( ))
		return;

	Handle->WaitUntilComplete( );
}
#endif