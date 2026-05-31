// Copyright Russell Aasland. All Rights Reserved.

#include "Strategy/StrategyHUDWidget.h"

#include "DataDefinitions/TacticalMapDefinition.h"

#include "GameWorld/CampaignGameMode.h"

#include "PersistentDataStore.h"
#include "DataStoreActors/BattleData.h"
#include "DataStoreActors/Campaign.h"
#include "DataStoreActors/GameplayRand.h"

#include "DataDefinitions/DataDefinitionLibrary.h"

// Engine
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StrategyHUDWidget)

void UStrategyHUDWidget::TransitionToTactical( )
{
	const auto DataStore = UPersistentDataStore::GetSubsystem( this );
	check( DataStore != nullptr );

	const auto GameplayRand = ADS_GameplayRand::GetSingleton( this );
	const auto Campaign = ADS_Campaign::GetSingleton( this );
	const auto BattleData = DataStore->SpawnSingleton< ADS_BattleData >( );

	auto AvailableHeroes = Campaign->ActiveHeroes;
	while (!AvailableHeroes.IsEmpty( ) && (BattleData->Squad.Num( ) < Campaign->SquadSize))
		BattleData->Squad.Push( GameplayRand->RemoveRandom( AvailableHeroes ) );

	const auto Library = UDataDefinitionLibrary::GetInstance( );

	const auto TacticalMaps = Library->GetAllDefinitions< UTacticalMapDefinition >( );

	if (const auto RandomMap = GameplayRand->PickRandom( TacticalMaps ))
		ACampaignGameMode::TransitionToNewGameplayLevel( this, RandomMap->GetLevel( ) );
}