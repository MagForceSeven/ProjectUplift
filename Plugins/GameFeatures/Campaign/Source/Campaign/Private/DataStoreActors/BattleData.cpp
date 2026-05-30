// Copyright Russell Aasland. All Rights Reserved.

#include "DataStoreActors/BattleData.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleData)

TSet< FPrimaryAssetId > ADS_BattleData::GatherAssetsForModeBundles_Implementation( const FGameplayTag &Mode ) const
{

	return Results;
}