// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "DataDefinitions/DefinitionExtension.h"

#include "CampaignDifficultyExtension.generated.h"

// An extension to difficulty assets for information related to single player campaign
UCLASS( )
class CAMPAIGN_API UCampaignDifficultyExtension : public UDataDefinitionExtension
{
	GENERATED_BODY( )
public:
	// Data Definition API
	void Verify( const UObject *WorldContext ) override;

#if WITH_EDITORONLY_DATA
	// Data Definition Extension
	[[nodiscard]] TArray< TSubclassOf< UDataDefinition > > GetCompatibleDefinitionTypes( void ) const override;
#endif
};