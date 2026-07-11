// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "DataStoreActor.h"
#include "GameFactsProviderInterface.h"
#include "UI/DisplayParamInterface.h"
#include "Framework/GameModeAssetProvider.h"

#include "Hero.generated.h"

class UHeroClassDefinition;

// A specification for creating a new hero character
USTRUCT( BlueprintType )
struct FHeroSpec
{
	GENERATED_BODY( )
public:
	// The class to make the new Hero
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSoftObjectPtr< const UHeroClassDefinition > ClassDefinition;

	// The name of the new Hero
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FText HeroName;

	// Check if the spec is configured properly to spawn a hero
	CAMPAIGN_API bool IsValid( void ) const;
};

// The hero characters controlled by the player
UCLASS( )
class CAMPAIGN_API ADS_Hero : public ADataStoreActor, public IGameFactsProvider, public IDisplayParamInterface,
								public IGameModeAssetProvider
{
	GENERATED_BODY( )
public:
	// Create a new hero
	UFUNCTION( BlueprintCallable, meta = (WorldContext = "WorldContext") )
	static ADS_Hero* SpawnHero( const UObject *WorldContext, const FHeroSpec &Spec );

	// Get the class of the Hero
	const UHeroClassDefinition* GetClassDefinition( void ) const { return ClassDefinition; }

	// Display Param Interface API
	[[nodiscard]] FText GetUIDisplayName( ) const override { return HeroName; }
	[[nodiscard]] UTexture2D* GetUIImage_Large( ) const override;
	[[nodiscard]] TSoftObjectPtr< UTexture2D > GetUIImage_Large_Soft( ) const override;

#if WITH_EDITOR
	FString GetCustomActorLabel_Implementation( ) const override;
	FString GetCustomOutlinerFolder_Implementation( ) const override;
#endif

	// Game Mode Asset Provider API
	TSet< FPrimaryAssetId > GatherAssetsForModeBundles_Implementation( const FGameplayTag &Mode ) const override;
	
protected:
	// The general class of the hero
	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly )
	TObjectPtr< const UHeroClassDefinition > ClassDefinition;

	// The name of the Hero
	UPROPERTY( VisibleInstanceOnly )
	FText HeroName;

	// Blueprint hook to respond to hero creation
	UFUNCTION( BlueprintImplementableEvent )
	void OnCreate( const FHeroSpec &Spec );

	// Game Facts Provider API
	void AppendGameFacts_Implementation( FGameplayTagContainer &OutFacts ) const override;
};