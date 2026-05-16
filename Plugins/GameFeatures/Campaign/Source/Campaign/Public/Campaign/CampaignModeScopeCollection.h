// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "ActorCollectionSingleton.h"
#include "Templates/ActorSingletonAccessor.h"

#include "CampaignModeScopeCollection.generated.h"

// A base class for singleton collections who's members should be destroyed when leaving the game mode
// (but they could persist across save/load normally)
UCLASS( Abstract )
class ACampaignModeScopeCollection : public AActorCollectionSingleton, public TActorSingletonAccessors< ACampaignModeScopeCollection >
{
	GENERATED_BODY( )
public:
};