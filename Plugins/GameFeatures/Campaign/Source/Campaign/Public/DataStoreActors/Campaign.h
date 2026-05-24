// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "DataStoreSingleton.h"
#include "Templates/ActorSingletonAccessor.h"

#include "GameplayTagContainer.h"

#include "Campaign.generated.h"

class UCampaignDifficultyExtension;

// General data for the overall campaign structure of the game
UCLASS( Blueprintable )
class CAMPAIGN_API ADS_Campaign : public ADataStoreSingleton, public TActorSingletonAccessors< ADS_Campaign >
{
	GENERATED_BODY( )
public:
	// Get the unique identifier for the overall campaign
	UFUNCTION( BlueprintCallable, meta = (WorldContext = "WorldContext") )
	[[nodiscard]] static FGuid GetCampaignID( const UObject* WorldContext );
	
	// Get the configuration for the campaigns difficulty settings
	[[nodiscard]] const UCampaignDifficultyExtension* GetDifficulty( void ) const { return Difficulty; }

private:
	friend class ACampaignGameMode;

	// Flags affecting the overall campaign
	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	FGameplayTagContainer CampaignFlags;

	// The difficulty selected for campaign
	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	TObjectPtr< const UCampaignDifficultyExtension > Difficulty;
};