// Copyright Russell Aasland. All Rights Reserved.

#include "Tactical/TacticalWorldSubsystems.h"

#include "GameWorld/UpliftWorldSettings.h"
#include "Tactical/TacticalGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TacticalWorldSubsystems)

bool UTacticalWorldSubsystem::ShouldCreateSubsystem( UObject *Outer ) const
{
	if (!Super::ShouldCreateSubsystem( Outer ))
		return false;

	const auto World = CastChecked< UWorld >( Outer );
	const auto Settings = CastChecked< AUpliftWorldSettings >( World->GetWorldSettings( ) );

	if (!Settings->GetWorldType( ).MatchesTag( ATacticalGameMode::WorldType_Tactical ))
		return false;

	return true;
}

bool UTacticalTickableWorldSubsystem::ShouldCreateSubsystem( UObject *Outer ) const
{
	if (!Super::ShouldCreateSubsystem( Outer ))
		return false;

	const auto World = CastChecked< UWorld >( Outer );
	const auto Settings = CastChecked< AUpliftWorldSettings >( World->GetWorldSettings( ) );

	if (!Settings->GetWorldType( ).MatchesTag( ATacticalGameMode::WorldType_Tactical ))
		return false;

	return true;
}
