// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

#include "CampaignGameInstance.generated.h"

class USimulationSettings;
class UNewGameSettings;

// Game Instance information relevant to the Campaign Game Feature plugin
UCLASS( )
class CAMPAIGN_API UCampaignGameInstance : public UGameInstanceSubsystem
{
	GENERATED_BODY( )
public:
	// Accessors
	[[nodiscard]] static UCampaignGameInstance* Get( const UObject *WorldContext );
	[[nodiscard]] static UCampaignGameInstance* Get( const UGameInstance *GameInstance );

#if !UE_BUILD_SHIPPING
	// Check if we think we're running a PIE session (checking the settings because the world could still be PIE if we launched from a PIE main menu)
	[[nodiscard]] bool IsQuickplay( void ) const { return (NewGameSettings == nullptr) && (SimulationSettings == nullptr); }
#endif

	// Collection of properties that can/should be filled out to start a new game
	UPROPERTY( EditInstanceOnly, BlueprintReadWrite )
	TObjectPtr< UNewGameSettings > NewGameSettings;

	// Collection of properties that can/should be filled out when starting a tactical simulation from the main menu
	UPROPERTY( EditInstanceOnly, BlueprintReadWrite )
	TObjectPtr< USimulationSettings > SimulationSettings;
};