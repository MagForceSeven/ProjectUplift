// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Campaign/CampaignModeScopeCollection.h"
#include "Templates/ActorSingletonAccessor.h"

#include "StrategyModeScopeCollection.generated.h"

// Collection of actors that should not be carried over from the Tactical Game Mode
UCLASS( )
class AStrategyModeScopeCollection : public ACampaignModeScopeCollection, public TActorSingletonAccessors< AStrategyModeScopeCollection >
{
	GENERATED_BODY( )
public:
};