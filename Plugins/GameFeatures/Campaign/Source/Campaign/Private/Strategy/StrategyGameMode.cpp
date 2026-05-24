// Copyright Russell Aasland. All Rights Reserved.

#include "Strategy/StrategyGameMode.h"

#include "Strategy/StrategyGameState.h"
#include "Strategy/StrategyPlayerController.h"
#include "Strategy/StrategyPlayerState.h"
#include "Strategy/StrategyHUD.h"
#include "Strategy/StrategyModeScopeCollection.h"

#include "CampaignBundles.h"
#include "Settings/CampaignSettings.h"

#include "SaveGames/UpliftCampaignSaveSubsystem.h"

#include "PersistentDataStore.h"
#include "DataStoreActors/BattleData.h"

// Engine
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StrategyGameMode)

UE_DEFINE_GAMEPLAY_TAG_COMMENT( AStrategyGameMode::WorldType_Strategy, "World.Type.Strategy", "A world supporting the strategy meta-game progression." );

AStrategyGameMode::AStrategyGameMode( )
{
	GameStateClass = AStrategyGameState::StaticClass( );
	PlayerControllerClass = AStrategyPlayerController::StaticClass( );
	PlayerStateClass = AStrategyPlayerState::StaticClass( );
	HUDClass = AStrategyHUD::StaticClass( );

	GameModeBundles.Push( CampaignBundles::Strategy );
}

void AStrategyGameMode::LaunchNewGame( const UObject *WorldContext )
{
	const auto Settings = GetDefault< UCampaignSettings >( );
	const auto StrategyPath = Settings->StrategyLevel.GetLongPackageFName( );
	
	UGameplayStatics::OpenLevel( WorldContext, StrategyPath );
}

void AStrategyGameMode::InitializeCommon( )
{
	Super::InitializeCommon( );

	const auto ScopeCollection = GetWorld( )->SpawnActor< AStrategyModeScopeCollection >( );
}

void AStrategyGameMode::TransitionIntoMode( )
{
	Super::TransitionIntoMode( );
}

void AStrategyGameMode::GameModeReady( )
{
	Super::GameModeReady( );

	if (UUpliftCampaignSaveSubsystem::GetSaveGameLoadingType( this ) == EExecGameLoading::LevelTransition)
	{
		const auto DataStore = UPersistentDataStore::GetSubsystem( this );
		check( DataStore != nullptr );

		const auto BattleData = DataStore->GetSingleton< ADS_BattleData >( );
		BattleData->Destroy( );
	}
}

void AStrategyGameMode::InitializeForQuickPlay( )
{
	Super::InitializeForQuickPlay( );
}

void AStrategyGameMode::PreTransitionOutOfMode( )
{
	Super::PreTransitionOutOfMode( );
}
