// Copyright Russell Aasland. All Rights Reserved.

#include "GameFeatures/UpliftGameFeatureProjectPolicy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftGameFeatureProjectPolicy)

TArray< FName > UUpliftGameFeatureProjectPolicy::AdditionalPreloadBundles;

const TArray< FName > UUpliftGameFeatureProjectPolicy::GetPreloadBundleStateForGameFeature( ) const
{
	auto Bundles = Super::GetPreloadBundleStateForGameFeature( );

	Bundles.Append( AdditionalPreloadBundles );

	return Bundles;
}