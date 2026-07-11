// Copyright Russell Aasland. All Rights Reserved.

#include "Tactical/TacticalWorldSubsystems.h"

#include "Tactical/TacticalGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TacticalWorldSubsystems)

bool UTacticalWorldSubsystem::DoesSupportWorldType( const FGameplayTag &Tag ) const
{
	if (!Super::DoesSupportWorldType( Tag ))
		return false;

	return Tag.MatchesTag( ATacticalGameMode::WorldType_Tactical );
}

bool UTacticalTickableWorldSubsystem::DoesSupportWorldType( const FGameplayTag &Tag ) const
{
	if (!Super::DoesSupportWorldType( Tag ))
		return false;

	return Tag.MatchesTag( ATacticalGameMode::WorldType_Tactical );
}