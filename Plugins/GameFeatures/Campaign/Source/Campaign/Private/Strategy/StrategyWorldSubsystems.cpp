// Copyright Russell Aasland. All Rights Reserved.

#include "Strategy/StrategyWorldSubsystems.h"

#include "Strategy/StrategyGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StrategyWorldSubsystems)

bool UStrategyWorldSubsystem::DoesSupportWorldType( const FGameplayTag &Tag ) const
{
	if (!Super::DoesSupportWorldType( Tag ))
		return false;

	return Tag.MatchesTag( AStrategyGameMode::WorldType_Strategy );
}

bool UStrategyTickableWorldSubsystem::DoesSupportWorldType( const FGameplayTag &Tag ) const
{
	if (!Super::DoesSupportWorldType( Tag ))
		return false;

	return Tag.MatchesTag( AStrategyGameMode::WorldType_Strategy );
}
