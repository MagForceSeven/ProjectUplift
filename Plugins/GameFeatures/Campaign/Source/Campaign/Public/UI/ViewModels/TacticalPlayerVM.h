// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "ActorViewModels/ActorVMSingleton.h"

#include "TacticalPlayerVM.generated.h"

class UHeroVM;

// A singleton VM referencing the general tactical data for the player
UCLASS( )
class CAMPAIGN_API UTacticalPlayerVM : public UActorVMSingleton
{
	GENERATED_BODY( )
public:

protected:
	// Actor VM API
	void HandleOnCreate( AActor *Actor ) override;

	// The display information VM's for the Heroes brought into tactical
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	TArray< TObjectPtr< UHeroVM > > HeroDisplayVMs;
};