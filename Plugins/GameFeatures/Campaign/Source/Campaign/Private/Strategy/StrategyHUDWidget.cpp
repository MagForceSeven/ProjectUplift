// Copyright Russell Aasland. All Rights Reserved.

#include "Strategy/StrategyHUDWidget.h"

#include "DataDefinitions/TacticalMapDefinition.h"

#include "GameWorld/CampaignGameMode.h"

#include "PersistentDataStore.h"
#include "DataStoreActors/BattleData.h"

#include "DataDefinitions/DataDefinitionLibrary.h"
#include "Templates/ContainerRandUtilities.h"

// Engine
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StrategyHUDWidget)

void UStrategyHUDWidget::TransitionToTactical( )
{
	const auto DataStore = UPersistentDataStore::GetSubsystem( this );
	check( DataStore != nullptr );

	DataStore->SpawnSingleton< ADS_BattleData >( );

	const auto Library = UDataDefinitionLibrary::GetInstance( );

	const auto TacticalMaps = Library->GetAllDefinitions< UTacticalMapDefinition >( );

	if (const auto RandomMap = ContainerRand::RandElement( TacticalMaps ))
		ACampaignGameMode::TransitionToNewGameplayLevel( this, RandomMap->GetLevel( ) );
}