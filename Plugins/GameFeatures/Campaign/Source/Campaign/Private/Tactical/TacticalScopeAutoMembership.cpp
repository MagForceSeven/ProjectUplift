// Copyright Russell Aasland. All Rights Reserved.

#include "Tactical/TacticalScopeAutoMembership.h"

#include "Tactical/TacticalModeScopeCollection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TacticalScopeAutoMembership)

TSubclassOf< AActorCollectionSingleton > UTacticalScopeAutoMembership::GetAssociatedCollectionClass( void ) const
{
	return ATacticalModeScopeCollection::StaticClass( );
}
