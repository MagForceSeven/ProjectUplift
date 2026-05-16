// Copyright Russell Aasland. All Rights Reserved.

#include "Shell/ShellGameMode.h"

#include "Shell/ShellGameState.h"
#include "Shell/ShellPlayerController.h"
#include "Shell/ShellPlayerState.h"
#include "Shell/ShellHUD.h"
#include "Shell/ShellPawn.h"

#include "DataDefinitions/DataDefinitionLibrary.h"
#include "GameFeatures/FeatureContentManager.h"
#include "Perf/SplatTaskManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ShellGameMode)

AShellGameMode::AShellGameMode( )
{
	GameStateClass = AShellGameState::StaticClass( );
	PlayerControllerClass = AShellPlayerController::StaticClass( );
	PlayerStateClass = AShellPlayerState::StaticClass( );
	DefaultPawnClass = AShellPawn::StaticClass( );
	HUDClass = AShellHUD::StaticClass( );

	ExtensionBundles.Push( Uplift_Bundles::MainMenu );

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AShellGameMode::HandleMatchIsWaitingToStart( )
{
	const auto FeaturesManager = UFeatureContentManager::GetSubsystem( this );

	auto InitializationSplat = [ this, FeaturesManager ]( ) -> ESplatTaskResult
	{
		switch (InitStage)
		{
			case EShellInitializationStage::Init:
				FeaturesManager->DisableAllFeatures( );
				InitStage = EShellInitializationStage::ToggleBundles;

			case EShellInitializationStage::ToggleBundles:
			{
				const auto Library = UDataDefinitionLibrary::GetInstance( );
				const auto OwnedFeatureIDs = FeaturesManager->GetOwnedFeatureIDs( );

				BundleHandle = Library->ChangeBundleStateForPrimaryAssets( OwnedFeatureIDs, ExtensionBundles, { } );

				if (BundleHandle.IsValid( ))
					InitStage = EShellInitializationStage::WaitOnBundles;
				else
					InitStage = EShellInitializationStage::Complete;
			} break;

			case EShellInitializationStage::WaitOnBundles:
				if (!BundleHandle->HasLoadCompleted( ))
					break;
				InitStage = EShellInitializationStage::Complete;

			case EShellInitializationStage::Complete:
				SetActorTickEnabled( true );
				Super::HandleMatchIsWaitingToStart( );
				return ESplatTaskResult::Complete;
		}

		return ESplatTaskResult::Yield;
	};

	const auto TaskManager = USplatTaskManager::GetSubsystem( this );
	TaskManager->StartTask( MoveTemp( InitializationSplat ) );
}

bool AShellGameMode::ReadyToStartMatch_Implementation( )
{
	if (InitStage != EShellInitializationStage::Complete)
		return false;
	
	return Super::ReadyToStartMatch_Implementation( );
}

void AShellGameMode::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	const auto FeaturesManager = UFeatureContentManager::GetSubsystem( this );
	const auto Library = UDataDefinitionLibrary::GetInstance( );

	const auto OwnedFeatureIDs = FeaturesManager->GetOwnedFeatureIDs( );

	Library->ChangeBundleStateForPrimaryAssets( OwnedFeatureIDs, { }, ExtensionBundles );

	Super::EndPlay( EndPlayReason );
}
