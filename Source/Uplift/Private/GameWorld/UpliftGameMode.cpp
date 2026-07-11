// Copyright Russell Aasland. All Rights Reserved.

#include "GameWorld/UpliftGameMode.h"

#include "GameWorld/UpliftGameState.h"
#include "GameWorld/UpliftPlayerController.h"
#include "GameWorld/UpliftPlayerState.h"
#include "GameWorld/UpliftHUD.h"
#include "GameWorld/UpliftPawn.h"

#include "Components/GameModeViewModels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftGameMode)

AUpliftGameMode::AUpliftGameMode( )
{
	GameStateClass = AUpliftGameState::StaticClass( );
	PlayerControllerClass = AUpliftPlayerController::StaticClass( );
	PlayerStateClass = AUpliftPlayerState::StaticClass( );
	DefaultPawnClass = AUpliftPawn::StaticClass( );
	HUDClass = AUpliftHUD::StaticClass( );

	DefaultViewModels = CreateDefaultSubobject< UGameModeViewModels >( TEXT( "DefaultViewModels" ) );
}