// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "ActorCollectionAutoMembershipComponent.h"

#include "TacticalScopeAutoMembership.generated.h"

// A component that will automatically add the Actor to the TacticalModeScopeCollection
UCLASS( )
class UTacticalScopeAutoMembership : public UActorCollectionAutoMembershipComponent
{
	GENERATED_BODY( )
protected:
	// Actor Collection Auto Membership Component API
	TSubclassOf< AActorCollectionSingleton > GetAssociatedCollectionClass( void ) const override;
};