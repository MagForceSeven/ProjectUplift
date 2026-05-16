// Copyright Russell Aasland. All Rights Reserved.

#include "Tactical/TacticalPlayerController.h"

#include "DefaultPlayerModeComponent.h"

#include "Tactical/PlayerModes/PlayerMode_TacticalIdle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TacticalPlayerController)

ATacticalPlayerController::ATacticalPlayerController( )
{
	DefaultPlayerMode = CreateDefaultSubobject< UDefaultPlayerModeComponent >( "DefaultPlayerMode" );
	DefaultPlayerMode->SetDefaultPlayerMode( APlayerMode_TacticalIdle::StaticClass( ) );
}
