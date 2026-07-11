// Copyright Russell Aasland. All Rights Reserved.

#include "Tactical/TacticalGameMode.h"

#include "Tactical/TacticalGameState.h"
#include "Tactical/TacticalPlayerController.h"
#include "Tactical/TacticalPlayerState.h"
#include "Tactical/TacticalHUD.h"
#include "Tactical/TacticalPawn.h"
#include "Tactical/TacticalModeScopeCollection.h"

#include "CampaignBundles.h"
#include "Campaign/CampaignGameInstance.h"
#include "SaveGames/UpliftCampaignSaveSubsystem.h"
#include "SaveGames/UpliftCampaignSaveUtilities.h"

#include "PersistentDataStore.h"
#include "DataStoreActors/Campaign.h"
#include "DataStoreActors/BattleData.h"

#include "GameFeatures/FeatureContentManager.h"
#include "GameFeatures/StarfireFeatureData.h"
#include "GameFeatures/Actions/GameFeatureAction_UpliftCampaign.h"

#include "Settings/CampaignPIESettings.h"


#include "Messenger/Messenger.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TacticalGameMode)

UE_DEFINE_GAMEPLAY_TAG_COMMENT( ATacticalGameMode::WorldType_Tactical, "World.Type.Tactical", "A world supporting the tactical detailed gameplay." );

ATacticalGameMode::ATacticalGameMode( )
{
	GameStateClass = ATacticalGameState::StaticClass( );
	PlayerControllerClass = ATacticalPlayerController::StaticClass( );
	PlayerStateClass = ATacticalPlayerState::StaticClass( );
	DefaultPawnClass = ATacticalPawn::StaticClass( );
	HUDClass = ATacticalHUD::StaticClass( );

	GameModeBundles.Push( CampaignBundles::Tactical );

	DefaultViewModels->DefaultViewModels.Push( UTacticalPlayerVM::StaticClass( ) );
}

void ATacticalGameMode::InitializeCommon( )
{
	Super::InitializeCommon( );

	const auto ScopeCollection = GetWorld( )->SpawnActor< ATacticalModeScopeCollection >( ); 
}

void ATacticalGameMode::InitializeForQuickPlay( )
{
	Super::InitializeForQuickPlay( );

	const auto DataStore = UPersistentDataStore::GetSubsystem( this );
	check( DataStore != nullptr );

	const auto CampaignInstance = UCampaignGameInstance::Get( this );
	check( CampaignInstance != nullptr );

	const auto BattleData = DataStore->SpawnSingleton< ADS_BattleData >( );
	const auto CampaignState = ADS_Campaign::GetSingleton( this );

	if (CampaignInstance->SimulationSettings != nullptr)
		BattleData->TacticalMode = ETacticalMode::Simulator;
	else
		BattleData->TacticalMode = ETacticalMode::PIE;

	const auto PIESettings = GetDefault< UCampaignPIESettings >( );

	for (const auto HeroSpec : PIESettings->AdditionalRoster)
	for (const auto &HeroSpec : PIESettings->AdditionalRoster)
	{
		if (!HeroSpec.IsValid( ))
			continue;

		const auto Hero = ADS_Hero::SpawnHero( this, HeroSpec );
		CampaignState->AddToRoster( Hero );
		BattleData->Squad.Push( Hero );

		if (BattleData->Squad.Num( ) >= CampaignState->SquadSize)
			break;
	}

	TransitionIntoMode( );
}

void ATacticalGameMode::TransitionIntoMode( )
{
	Super::TransitionIntoMode( );

	const auto BattleData = ADS_BattleData::GetSingleton( this );
	check( BattleData != nullptr );
	check( !BattleData->Squad.IsEmpty( ) );

	const auto CampaignState = ADS_Campaign::GetSingleton( this );

	ensureAlways( BattleData->Squad.Num( ) == CampaignState->SquadSize );

	// ---------------------------------------------------------------------------------------------------------------
	//   Allow the active features to add to the configuration of the new tactical session
	const auto FeaturesManager = UFeatureContentManager::GetSubsystem( this ); check( FeaturesManager != nullptr );
	const auto ActiveFeatures = FeaturesManager->GetEnabledFeatures( );

	const auto World = GetWorld( );
	for (const auto Feature : ActiveFeatures)
	{
		if (!ensureAlways( Feature != nullptr ))
			continue;

		for (const auto Action : Feature->GetActions( ))
		{
			const auto CampaignAction = Cast< UGameFeatureAction_UpliftCampaign >( Action );
			if (CampaignAction == nullptr)
				continue;

			CampaignAction->OnStartTactical( World );			
		}
	}
}

void ATacticalGameMode::GameModeReady( )
{
	Super::GameModeReady( );

	// TODO: Move this to someplace else as more tactical structure is put into place
	const auto OnCheckpointComplete = FCreateCheckpointComplete::CreateLambda( [ ]( const UObject *WorldContext, const UUpliftCampaignSave *CheckpointData, bool Success )
	{
		if (ensureAlways( Success ))
		{
			const auto SaveSubsystem = UUpliftCampaignSaveSubsystem::GetSubsystem( WorldContext );
			check( SaveSubsystem != nullptr );

			SaveSubsystem->TacticalStartCheckpoint = CheckpointData;
							
			UUpliftCampaignSaveUtilities::SaveCheckpointToSlot_Async( WorldContext, SaveSubsystem->TacticalStartCheckpoint, "TacticalStart", 0, ESaveGameType::Developer, "Developer - Mission Start" );
		}
	});
	UUpliftCampaignSaveUtilities::CreateCheckpointSave_Async( this, OnCheckpointComplete );

	UStarfireMessenger::GetSubsystem( this )->Broadcast< FMessage_TacticalModeReady >( );
}

void ATacticalGameMode::PreTransitionOutOfMode( )
{
	Super::PreTransitionOutOfMode( );

	const auto SaveSubsystem = UUpliftCampaignSaveSubsystem::GetSubsystem( this );
	check( SaveSubsystem != nullptr );

	SaveSubsystem->TacticalStartCheckpoint = nullptr;
}

TSet< FPrimaryAssetId > ATacticalGameMode::GatherAssetsForModeBundles( void ) const
{
	auto Assets = Super::GatherAssetsForModeBundles( );

	const auto BattleData = ADS_BattleData::GetSingleton( this );
	check( BattleData != nullptr );

	Assets.Append( IGameModeAssetProvider::Execute_GatherAssetsForModeBundles( BattleData, WorldType_Tactical ) );

	return Assets;
}
