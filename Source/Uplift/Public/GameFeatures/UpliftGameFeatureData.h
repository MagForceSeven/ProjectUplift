// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameFeatures/StarfireFeatureData.h"

#include "UpliftGameFeatureData.generated.h"

// Asset for additional information regarding content packages
UCLASS( )
class UPLIFT_API UUpliftGameFeatureData : public UStarfireFeatureData
{
	GENERATED_BODY( )
public:
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( ContentType_Campaign );
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( ContentType_Developer );
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( ContentFlag_NewGameOnly );
};