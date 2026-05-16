// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "ActorCollectionAutoMembershipComponent.h"

#include "StrategyScopeAutoMembership.generated.h"

// A component that will automatically add the Actor to the StrategyModeScopeCollection
UCLASS( )
class UStrategyScopeAutoMembership : public UActorCollectionAutoMembershipComponent
{
	GENERATED_BODY( )
protected:
	// Actor Collection Auto Membership Component API
	TSubclassOf< AActorCollectionSingleton > GetAssociatedCollectionClass( void ) const override;
};