// Copyright Russell Aasland. All Rights Reserved.

#include "DataDefinitions/CampaignDifficultyDefinition.h"

#include "DataDefinitions/DataDefinitionLibrary.h"

#include "Kismet/BlueprintUtilitiesSF.h"

#include "AssetValidation/AssetChecks.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CampaignDifficultyDefinition)

void UCampaignDifficultyDefinition::Verify( const UObject *WorldContext )
{
	Super::Verify( WorldContext );

	if (DisplayName.IsEmpty( ))
		AssetChecks::AC_Message( this, TEXT( "No Display Name configured for Difficulty" ), WorldContext );
}

const UCampaignDifficultyDefinition* UCampaignDifficultyDefinition::FindDefaultDifficulty( )
{
	for (TDataDefinitionIterator< UCampaignDifficultyDefinition > It; It; ++It)
	{
		if (It->bDefaultDifficulty)
			return *It;
	}

	return nullptr;
}

void UCampaignDifficultyDefinition::SortDifficulties( TArray< const UCampaignDifficultyDefinition* > &Array )
{
	const auto Predicate = [ ]( const UCampaignDifficultyDefinition &Lhs, const UCampaignDifficultyDefinition &Rhs )
	{
		return Lhs.SortingOrder < Rhs.SortingOrder;
	};
	
	Array.Sort( Predicate );
}

DEFINE_FUNCTION( UCampaignDifficultyDefinition::execSortDifficulties )
{
	// ReSharper disable once CppLocalVariableMayBeConst
	P_GET_TARRAY_REF( UCampaignDifficultyDefinition*, Z_Param_Out_Array );
	P_FINISH;
	P_NATIVE_BEGIN;
	UCampaignDifficultyDefinition::SortDifficulties( NativeCompatibilityCast( Z_Param_Out_Array ) );
	P_NATIVE_END;
}