// Copyright Russell Aasland. All Rights Reserved.

#include "DataDefinitions/Extensions/CampaignDifficultyExtension.h"

#include "DataDefinitions/CampaignDifficultyDefinition.h"

void UCampaignDifficultyExtension::Verify( const UObject *WorldContext )
{
	Super::Verify( WorldContext );
}

#if WITH_EDITOR
TArray< TSubclassOf< UDataDefinition > > UCampaignDifficultyExtension::GetCompatibleDefinitionTypes( ) const
{
	return { UCampaignDifficultyDefinition::StaticClass( ) };
}
#endif