// Copyright Russell Aasland. All Rights Reserved.

#include "UI/SquadSelectScreen.h"

#include "UI/ViewModels/MissionDeploymentVM.h"
#include "UI/ViewModels/HeroVM.h"
#include "DataDefinitions/TacticalMapDefinition.h"

#include "GameWorld/CampaignGameMode.h"

#include "PersistentDataStore.h"
#include "DataStoreActors/BattleData.h"
#include "DataStoreActors/Campaign.h"
#include "DataStoreActors/GameplayRand.h"
#include "DataStoreActors/Hero.h"

#include "DataDefinitions/DataDefinitionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SquadSelectScreen)

void USquadSelectScreen::TransitionToTactical( const UMissionDeploymentVM *VM )
{
	const auto DataStore = UPersistentDataStore::GetSubsystem( this );
	check( DataStore != nullptr );

	const auto BattleData = DataStore->SpawnSingleton< ADS_BattleData >( );

	for (const auto HeroVM : VM->GetSelectedHeroes( ))
	{
		const auto Hero = CastChecked< ADS_Hero >( HeroVM->GetActor( ) );
		BattleData->Squad.Push( Hero );
	}

	const auto Library = UDataDefinitionLibrary::GetInstance( );

	const auto TacticalMaps = Library->GetAllDefinitions< UTacticalMapDefinition >( );

	const auto GameplayRand = ADS_GameplayRand::GetSingleton( this );
	if (const auto RandomMap = GameplayRand->PickRandom( TacticalMaps ))
		ACampaignGameMode::TransitionToNewGameplayLevel( this, RandomMap->GetLevel( ) );
}
