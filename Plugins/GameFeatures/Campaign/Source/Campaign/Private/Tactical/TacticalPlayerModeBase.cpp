// Copyright Russell Aasland. All Rights Reserved.

#include "Tactical/TacticalPlayerModeBase.h"

#include "Tactical/TacticalPawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TacticalPlayerModeBase)

ATacticalPlayerModeBase::ATacticalPlayerModeBase( )
{
	PawnClass = ATacticalPawn::StaticClass( );
}