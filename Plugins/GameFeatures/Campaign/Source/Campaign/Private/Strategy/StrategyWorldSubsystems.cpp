// Copyright Russell Aasland. All Rights Reserved.

#include "Strategy/StrategyWorldSubsystems.h"

#include "GameWorld/UpliftWorldSettings.h"
#include "Strategy/StrategyGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StrategyWorldSubsystems)

bool UStrategyWorldSubsystem::ShouldCreateSubsystem( UObject *Outer ) const
{
	if (!Super::ShouldCreateSubsystem( Outer ))
		return false;

	const auto World = CastChecked< UWorld >( Outer );
	const auto Settings = CastChecked< AUpliftWorldSettings >( World->GetWorldSettings( ) );

	if (!Settings->GetWorldType( ).MatchesTag( AStrategyGameMode::WorldType_Strategy ))
		return false;

	return true;
}

bool UStrategyTickableWorldSubsystem::ShouldCreateSubsystem( UObject *Outer ) const
{
	if (!Super::ShouldCreateSubsystem( Outer ))
		return false;

	const auto World = CastChecked< UWorld >( Outer );
	const auto Settings = CastChecked< AUpliftWorldSettings >( World->GetWorldSettings( ) );

	if (!Settings->GetWorldType( ).MatchesTag( AStrategyGameMode::WorldType_Strategy ))
		return false;

	return true;
}
