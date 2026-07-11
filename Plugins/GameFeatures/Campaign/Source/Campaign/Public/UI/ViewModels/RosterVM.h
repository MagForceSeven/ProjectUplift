// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "ActorViewModels/ActorVMSingleton.h"

#include "RosterVM.generated.h"

struct FMessage_Roster_NewHero;

class UHeroVM;
class ADS_Hero;

// A singleton VM for the overall roster of active heroes the player has available
UCLASS( )
class CAMPAIGN_API URosterVM : public UActorVMSingleton
{
	GENERATED_BODY( )
public:

protected:
	// Actor VM API
	void HandleOnCreate( AActor *Actor ) override;
	void HandleOnDestroy( ) override;

	// The display information VM's for the Heroes available to the player
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	TArray< TObjectPtr< UHeroVM > > HeroDisplayVMs;

	// New hero being added to the roster
	UPROPERTY( FieldNotify )
	TObjectPtr< UHeroVM > NewHero;

	// Handle heroes being added to the roster
	void OnRosterAdd( const FMessage_Roster_NewHero &Message );
};