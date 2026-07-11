// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "DataStoreSingleton.h"
#include "Templates/ActorSingletonAccessor.h"

#include "Messenger/MessageTypes.h"

#include "GameplayTagContainer.h"

#include "Campaign.generated.h"

class UCampaignDifficultyExtension;
class ADS_Hero;

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

	// Get the heroes that make up the available roster of heroes
	[[nodiscard]] TArray< ADS_Hero* > GetRoster( void ) const { return ActiveHeroes; }

	// Add a new hero to the active roster
	UFUNCTION( BlueprintCallable )
	void AddToRoster( ADS_Hero *NewHero );
	
	// Whether or not to re-randomize the gameplay random number stream after loading a save
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite )
	bool bRandomizeSeedOnLoad = false;

	// The number of heroes sent into tactical
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
	int SquadSize = 3;

private:
	friend class ACampaignGameMode;

	// Flags affecting the overall campaign
	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	FGameplayTagContainer CampaignFlags;

	// The difficulty selected for campaign
	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	TObjectPtr< const UCampaignDifficultyExtension > Difficulty;

	// The Heroes currently available to the player
	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	TArray< TObjectPtr< ADS_Hero > > ActiveHeroes;

	// Blueprint Accessor - TODO: Replace with custom blueprint node
	UFUNCTION( BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContext") )
	static ADS_Campaign* GetCampaign( const UObject *WorldContext );
};

// Message indicating the addition of a new hero to the player's roster
USTRUCT( )
struct FMessage_Roster_NewHero : public FSf_Message_Immediate
{
	GENERATED_BODY( )
public:
	// The hero that was added to the roster
	UPROPERTY( BlueprintReadOnly )
	TObjectPtr< ADS_Hero > NewHero;
};