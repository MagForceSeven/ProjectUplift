// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Campaign/CampaignModeScopeCollection.h"
#include "Templates/ActorSingletonAccessor.h"

#include "TacticalModeScopeCollection.generated.h"

// Collection of actors that should not be carried over from the Tactical Game Mode
UCLASS( )
class ATacticalModeScopeCollection : public ACampaignModeScopeCollection, public TActorSingletonAccessors< ATacticalModeScopeCollection >
{
	GENERATED_BODY( )
public:
};