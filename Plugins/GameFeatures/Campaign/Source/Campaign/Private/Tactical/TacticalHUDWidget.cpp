// Copyright Russell Aasland. All Rights Reserved.

#include "Tactical/TacticalHUDWidget.h"

#include "Settings/CampaignSettings.h"
#include "GameWorld/CampaignGameMode.h"

#include "DataStoreActors/BattleData.h"

#include "PersistentDataStore.h"

// Engine
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TacticalHUDWidget)

void UTacticalHUDWidget::TransitionToStrategy( )
{
	const auto DataStore = UPersistentDataStore::GetSubsystem( this );
	check( DataStore != nullptr );

	const auto BattleData = DataStore->GetSingleton< ADS_BattleData >( );

	if (BattleData->TacticalMode == ETacticalMode::PIE)
	{
		const auto PlayerController = UGameplayStatics::GetPlayerController( this, 0 );
		PlayerController->ConsoleCommand( "quit" );

		return;
	}

	const auto Settings = GetDefault< UCampaignSettings >( );

	if (BattleData->TacticalMode == ETacticalMode::Simulator)
	{
		const auto ShellPath = Settings->ShellLevel.GetLongPackageFName( );
		UGameplayStatics::OpenLevel( this, ShellPath );
	}

	ACampaignGameMode::TransitionToNewGameplayLevel( this, Settings->StrategyLevel );
}