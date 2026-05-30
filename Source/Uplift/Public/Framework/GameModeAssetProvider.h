// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"

#include "GameModeAssetProvider.generated.h"

struct FGameplayTag;

// Static class for interfaces meant to be implemented by actors that can provide the game mode with assets to modify the bundle state of
UINTERFACE( MinimalAPI )
class UGameModeAssetProvider : public UInterface
{
	GENERATED_BODY( )
public:
};

// Actual interface class to be implemented by actors that provide game modes with asset collections
class UPLIFT_API IGameModeAssetProvider
{
	GENERATED_BODY( )
public:
	// Hook for the provider to gather the assets that should have bundles updated for the active game mode
	UFUNCTION( BlueprintNativeEvent )
	[[nodiscard]] TSet< FPrimaryAssetId > GatherAssetsForModeBundles( const FGameplayTag &Mode ) const;
	virtual TSet< FPrimaryAssetId > GatherAssetsForModeBundles_Implementation( const FGameplayTag &Mode ) const { return { }; }
};