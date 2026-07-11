// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "DataDefinitions/DataDefinition.h"
#include "UI/DisplayParamInterface.h"

#include "NativeGameplayTags_SF.h"

#include "GameplayTags.h"

#include "HeroClassDefinition.generated.h"

// The general type of classification available to heroes
UCLASS( )
class CAMPAIGN_API UHeroClassDefinition : public UDataDefinition, public IDisplayParamInterface
{
	GENERATED_BODY( )
public:
	// Player facing name for the character class
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Display" )
	FText DisplayName;

	// The icon used to represent heroes of this class
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Display", meta = (AssetBundles = "DebugAlwaysLoaded") )
	TSoftObjectPtr< UTexture2D > Icon;

	UE_DECLARE_GAMEPLAY_TAG_SCOPED( ClassTag_Root )
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( ClassTag_Mason )
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( ClassTag_Engineer )
	UE_DECLARE_GAMEPLAY_TAG_SCOPED( ClassTag_General )
	
	// A unique tag for identifying this class by tag
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Character Class", meta = (Categories = "Hero.Class") )
	FGameplayTag ClassID;

	// Display Param API
	[[nodiscard]] FText GetUIDisplayName() const override { return DisplayName; }
	[[nodiscard]] UTexture2D* GetUIImage_Large( ) const override { return Icon.Get( ); }
	[[nodiscard]] TSoftObjectPtr< UTexture2D > GetUIImage_Large_Soft( ) const override { return Icon; }

	// Verifiable Asset API
	void Verify( const UObject *WorldContext ) override;

#if WITH_EDITORONLY_DATA
protected:
	// Data Definition API
	[[nodiscard]] const UTexture2D* GetThumbnail( ) const override;
#endif
};