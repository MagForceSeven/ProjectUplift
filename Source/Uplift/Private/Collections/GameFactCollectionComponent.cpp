
#include "Collections/GameFactCollectionComponent.h"

#include "ActorCollectionBase.h"
#include "ActorCollectionMembershipComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFactCollectionComponent)

void UGameFactCollectionComponent::OnRegister( )
{
	Super::OnRegister( );

	const auto OwnerCollection = Cast< AActorCollectionBase >( GetOwner( ) );
	if (!IsValid( OwnerCollection ))
		return;

	const auto ActorAddedLambda = [ ]( AActor *NewActor, AActorCollectionBase *Collection ) -> void
	{
		auto FactComponent = NewActor->GetComponentByClass< UGameFactCollectionMembershipComponent >( );
		if (IsValid( FactComponent ))
			return;

		FactComponent = NewObject< UGameFactCollectionMembershipComponent >( NewActor );
		NewActor->AddOwnedComponent( FactComponent );
		FactComponent->RegisterComponent( );
	};

	// On add, make sure they have the component
	OwnerCollection->OnActorAdded.AddWeakLambda( this, ActorAddedLambda );

	// Make sure any existing members also have the component
	for (const auto Actor : OwnerCollection->GetCollectionMembers( ))
		ActorAddedLambda( Actor, OwnerCollection );
}

void UGameFactCollectionMembershipComponent::AppendGameFacts_Implementation( FGameplayTagContainer &OutFacts ) const
{
	const auto Owner = GetOwner( );
	if (!IsValid( Owner ))
		return;

	const auto CollectionMembership = Owner->GetComponentByClass< UActorCollectionMembershipComponent >( );
	if (!IsValid( CollectionMembership ))
		return;

	// Go through member collections getting facts
	// We don't use the generalized utility to add all facts from the collection as those may not be meant to apply to members of the collection
	for (const auto Collection : CollectionMembership->GetCollections( ))
	{
		if (const auto CollectionFacts = Collection->GetComponentByClass< UGameFactCollectionComponent >( ))
			OutFacts.AppendTags( CollectionFacts->GameFacts );
	}
}
