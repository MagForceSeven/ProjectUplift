// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "StarfireWorldSettings.h"

#include "UpliftWorldSettings.generated.h"

// Project specific world settings
UCLASS( )
class UPLIFT_API AUpliftWorldSettings : public AStarfireWorldSettings
{
	GENERATED_BODY( )
public:
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( WorldType_Shell )
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( WorldType_DevShell )
};