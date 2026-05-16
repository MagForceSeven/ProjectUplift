// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameFeatures/StarfireGameFeaturePolicy.h"

#include "UpliftGameFeatureProjectPolicy.generated.h"

// A game features policy to control feature content loading specifics for Uplift
UCLASS( )
class UPLIFT_API UUpliftGameFeatureProjectPolicy : public UStarfire_GameFeaturesProjectPolicy
{
	GENERATED_BODY( )
public:
	// Game Feature Project Policy API
	const TArray< FName > GetPreloadBundleStateForGameFeature( ) const override;

	// Project specific bundles that should be loaded as part of primary asset pre-loading
	static TArray< FName > AdditionalPreloadBundles;
};