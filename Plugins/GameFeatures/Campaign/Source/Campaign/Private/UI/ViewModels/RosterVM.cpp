// Copyright Russell Aasland. All Rights Reserved.

#include "UI/ViewModels/RosterVM.h"

#include "DataStoreActors/Campaign.h"
#include "DataStoreActors/Hero.h"

#include "ActorViewModels/ActorVMUtilities.h"
#include "UI/ViewModels/HeroVM.h"

#include "Messenger/Messenger.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RosterVM)

void URosterVM::HandleOnCreate( AActor *Actor )
{
	Super::HandleOnCreate( Actor );

	const auto Campaign = ADS_Campaign::GetSingleton( this );
	for (const auto &Hero : Campaign->GetRoster( ))
		HeroDisplayVMs.Push( UActorVMUtilities::FindOrCreateVM< UHeroVM >( Hero ) );
	
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( HeroDisplayVMs );

	UStarfireMessenger::GetSubsystem( Actor )->StartListeningForMessage< FMessage_Roster_NewHero >( this, &URosterVM::OnRosterAdd );
}

void URosterVM::HandleOnDestroy( )
{
	UStarfireMessenger::GetSubsystem( this )->StopListeningForAllMessages( this );
	
	Super::HandleOnDestroy( );
}

void URosterVM::OnRosterAdd( const FMessage_Roster_NewHero &Message )
{
	NewHero = UActorVMUtilities::FindOrCreateVM< UHeroVM >( Message.NewHero );
	HeroDisplayVMs.Push( NewHero );

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( NewHero );

	NewHero = nullptr;

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( HeroDisplayVMs );
}
