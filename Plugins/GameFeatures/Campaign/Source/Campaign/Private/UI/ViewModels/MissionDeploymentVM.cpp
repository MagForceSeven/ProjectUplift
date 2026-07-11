// Copyright Russell Aasland. All Rights Reserved.

#include "UI/ViewModels/MissionDeploymentVM.h"

#include "DataStoreActors/Campaign.h"
#include "DataStoreActors/Hero.h"

#include "ActorViewModels/ActorVMUtilities.h"
#include "UI/ViewModels/HeroVM.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MissionDeploymentVM)

void UMissionDeploymentVM::PostInitProperties( )
{
	Super::PostInitProperties( );
	
	if (IsTemplate( ))
		return;
	
	const auto Campaign = ADS_Campaign::GetSingleton( this );

	DeploymentSize = Campaign->SquadSize;

	for (const auto Hero :  Campaign->GetRoster( ))
		PossibleHeroes.Push( UActorVMUtilities::FindOrCreateVM< UHeroVM >( Hero ) );

	const int MaxCount = FMath::Min( DeploymentSize, PossibleHeroes.Num( ) );
	HeroSelections.Append( PossibleHeroes.GetData( ), MaxCount );

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( DeploymentSize );
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( PossibleHeroes );
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( HeroSelections );
}

bool UMissionDeploymentVM::IsDeploymentAllowed( ) const
{
	return !HeroSelections.Contains( nullptr );
}

TArray< UHeroVM* > UMissionDeploymentVM::GetPossibleHeroes( int Index ) const
{
	if (!ensureAlways( HeroSelections.IsValidIndex( Index ) ))
		return { };

	// Start with all heroes
	auto SlotPossibles = PossibleHeroes;

	// Remove everyone already selected in other slots
	for (int i = 0; i < HeroSelections.Num( ); ++i)
	{
		if (i == Index)
			continue;

		SlotPossibles.Remove( HeroSelections[ i ] );
	}

	return SlotPossibles;
}

void UMissionDeploymentVM::SetHeroSelection( int Index, UHeroVM *HeroVM )
{
	if (!ensureAlways( HeroSelections.IsValidIndex( Index ) ))
		return;

	HeroSelections[ Index ] = HeroVM;
}
