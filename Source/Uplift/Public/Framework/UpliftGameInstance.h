// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "StarfireGameInstance.h"

#include "UpliftGameInstance.generated.h"

// UE class that is created at game (or PIE) start and exists until game (or PIE) termination
UCLASS( )
class UPLIFT_API UUpliftGameInstance : public UStarfireGameInstance
{
	GENERATED_BODY( )
public:
	// Accessor to the game specific version of the game instance object
	UFUNCTION( BlueprintCallable, BlueprintPure = true, meta = (DisplayName = "Get Uplift Game Instance", WorldContext = "WorldContext"))
	[[nodiscard]] static UUpliftGameInstance* GetInstance( const UObject *WorldContext );
};