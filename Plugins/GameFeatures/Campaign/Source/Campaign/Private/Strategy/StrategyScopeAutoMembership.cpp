// Copyright Russell Aasland. All Rights Reserved.

#include "Strategy/StrategyScopeAutoMembership.h"

#include "Strategy/StrategyModeScopeCollection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StrategyScopeAutoMembership)

TSubclassOf< AActorCollectionSingleton > UStrategyScopeAutoMembership::GetAssociatedCollectionClass( ) const
{
	return AStrategyModeScopeCollection::StaticClass( );
}
