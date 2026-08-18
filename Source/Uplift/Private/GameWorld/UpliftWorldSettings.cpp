// Copyright Russell Aasland. All Rights Reserved.

#include "GameWorld/UpliftWorldSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftWorldSettings)

UE_DEFINE_GAMEPLAY_TAG_COMMENT( AUpliftWorldSettings::WorldType_Shell, "World.Type.Shell", "A world representing the main menu." );
UE_DEFINE_GAMEPLAY_TAG_COMMENT( AUpliftWorldSettings::WorldType_DevShell, "World.Type.Shell.Dev", "A world representing a developer-only version of a main menu." );

const AUpliftWorldSettings* AUpliftWorldSettings::GetWorldSettings( const UObject *WorldContext )
{
	return CastChecked< AUpliftWorldSettings >( Super::GetWorldSettings( WorldContext ), ECastCheckedType::NullAllowed );
}

const AUpliftWorldSettings* AUpliftWorldSettings::GetWorldSettings( const UWorld *World )
{
	return CastChecked< AUpliftWorldSettings >( Super::GetWorldSettings( World ), ECastCheckedType::NullAllowed );
}