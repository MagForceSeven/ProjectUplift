// Copyright Russell Aasland. All Rights Reserved.

#include "Tactical/PlayerModes/PlayerMode_TacticalIdle.h"

#include "Tactical/PlayerModes/TacticalIdlePawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerMode_TacticalIdle)

APlayerMode_TacticalIdle::APlayerMode_TacticalIdle( )
{
	PawnClass = ATacticalIdlePawn::StaticClass( );
}