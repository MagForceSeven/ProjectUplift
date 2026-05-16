// Copyright Russell Aasland. All Rights Reserved.

#include "DataDefinitions/TacticalMapDefinition.h"

#include "GameWorld/UpliftWorldSettings.h"
#include "Tactical/TacticalGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TacticalMapDefinition)

#if WITH_EDITOR
bool UTacticalMapDefinition::ShouldCreateMetadata( const UWorld *World ) const
{
	if (!IsValid( World ))
		return false;

	const auto WorldSettings = CastChecked< AUpliftWorldSettings >( World->GetWorldSettings( ) );

	return WorldSettings->GetWorldType( ).MatchesTag( ATacticalGameMode::WorldType_Tactical );
}

void UTacticalMapDefinition::InitializeMetadata( const UWorld *World )
{
	Super::InitializeMetadata( World );

	// TODO: Copy data from the world
}
#endif