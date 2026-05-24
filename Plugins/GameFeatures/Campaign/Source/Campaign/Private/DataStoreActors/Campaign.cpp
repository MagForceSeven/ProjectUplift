// Copyright Russell Aasland. All Rights Reserved.

#include "DataStoreActors/Campaign.h"

#include "PersistenceComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Campaign)

FGuid ADS_Campaign::GetCampaignID( const UObject *WorldContext )
{
	const auto Campaign = ADS_Campaign::GetSingleton( WorldContext );
	check( Campaign != nullptr );

	const auto Component = Campaign->GetComponentByClass< UPersistenceComponent >( );
	check( Component != nullptr );

	return Component->GetGuid( );
}
