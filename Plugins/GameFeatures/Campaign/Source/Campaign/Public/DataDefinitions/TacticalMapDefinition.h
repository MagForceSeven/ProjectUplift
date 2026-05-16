// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "LevelMetadata.h"

#include "TacticalMapDefinition.generated.h"

// Metadata about tactical map locations
UCLASS( )
class CAMPAIGN_API UTacticalMapDefinition : public ULevelMetadata
{
	GENERATED_BODY( )
public:

protected:
#if WITH_EDITOR
	// Level Metadata API
	[[nodiscard]] bool ShouldCreateMetadata( const UWorld *World ) const override;
	void InitializeMetadata( const UWorld *World ) override;
#endif
};