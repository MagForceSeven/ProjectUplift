// Copyright Russell Aasland. All Rights Reserved.

#include "GameWorld/UpliftWorldSubsystems.h"

#include "GameWorld/UpliftWorldSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftWorldSubsystems)

bool UUpliftWorldSubsystem::ShouldCreateSubsystem( UObject *Outer ) const
{
	if (!Super::ShouldCreateSubsystem( Outer ))
		return false;

	const auto World = CastChecked< UWorld >( Outer );
	const auto Settings = CastChecked< AUpliftWorldSettings >( World->GetWorldSettings( ) );
	
	if (DoesSupportWorldType( Settings->GetWorldType( ) ))
		return true;
		
	return false;
}

bool UUpliftTickableWorldSubsystem::ShouldCreateSubsystem( UObject *Outer ) const
{
	if (!Super::ShouldCreateSubsystem( Outer ))
		return false;

	const auto World = CastChecked< UWorld >( Outer );
	const auto Settings = CastChecked< AUpliftWorldSettings >( World->GetWorldSettings( ) );

	if (DoesSupportWorldType( Settings->GetWorldType( ) ))
		return true;
		
	return false;
}
