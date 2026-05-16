
#pragma once

#include "Components/ActorComponent.h"
#include "GameFactsProviderInterface.h"

#include "GameFactCollectionComponent.generated.h"

// A component that can be applied to Actor Collections that will apply the specified tags to any members of the collection
UCLASS( meta = (BlueprintSpawnableComponent) )
class UPLIFT_API UGameFactCollectionComponent : public UActorComponent
{
	GENERATED_BODY( )
public:
	// Actor Component API
	void OnRegister( ) override;

private:
	friend class UGameFactCollectionMembershipComponent;
	
	// The Facts that should be applied
	UPROPERTY( EditDefaultsOnly )
	FGameplayTagContainer GameFacts;
};

// A component added to actors that are members of Actor Collections with a Game Fact Collection Component
// This component implements the Provider interface and adds any facts from Game Fact Collection Components which
//		exist on the collections the actor is a member of
UCLASS( )
class UGameFactCollectionMembershipComponent : public UActorComponent, public IGameFactsProvider
{
	GENERATED_BODY( )
public:

protected:
	// Game Facts Provider API
	void AppendGameFacts_Implementation( FGameplayTagContainer &OutFacts ) const override;
};