// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/UpliftGameMode.h"

#include "ShellGameMode.generated.h"

struct FStreamableHandle;

// Series of initialization steps before the game mode is ready to start the match
enum class EShellInitializationStage
{
	Init,
	ToggleBundles,
	WaitOnBundles,
	Complete
};

namespace Uplift_Bundles
{
	static const FName MainMenu( "MainMenu" );
}

// Shell related game mode
UCLASS( )
class UPLIFT_API AShellGameMode : public AUpliftGameMode
{
	GENERATED_BODY( )
public:
	AShellGameMode( );

	// Game Mode API
	void HandleMatchIsWaitingToStart( ) override;
	bool ReadyToStartMatch_Implementation( ) override;

	// Actor API
	void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

private:
	// The current initialization step of the mode
	EShellInitializationStage InitStage = EShellInitializationStage::Init;

	// The handle to the streaming request made for the game feature bundles
	TSharedPtr< FStreamableHandle > BundleHandle;

	// The bundles that are enabled on owned feature content assets to extend the Main Menu options
	UPROPERTY( BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	TArray< FName > ExtensionBundles;
};