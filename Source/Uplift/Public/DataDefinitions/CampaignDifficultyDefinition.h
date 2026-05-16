// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "DataDefinitions/DataDefinition.h"

#include "CampaignDifficultyDefinition.generated.h"

// Information related to overall game difficulty
UCLASS( )
class UPLIFT_API UCampaignDifficultyDefinition : public UDataDefinition
{
	GENERATED_BODY( )
public:
	// A user-facing name to describe this difficulty
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Display" )
	FText DisplayName;

	// The order in which this category should be displayed relative to other instances of the same type
	UPROPERTY( EditDefaultsOnly, Category = "Display" )
	int SortingOrder = 0;

	// Whether or not this should be considered the "default".
	// Will generally be the initial selection when dealing with a group of Difficulty Categories
	UPROPERTY( EditDefaultsOnly, Category = "Display" )
	bool bDefaultDifficulty = false;

	// Utility to find the default category within the set of all categories
	UFUNCTION( BlueprintCallable, Category = "Difficulty" )
	[[nodiscard]] static const UCampaignDifficultyDefinition* FindDefaultDifficulty( );

	// Sort a collection of difficulties based on the defined sorting order from the definition
	UFUNCTION( BlueprintCallable, Category = "Difficulty", CustomThunk )
	static void SortDifficulties( UPARAM( ref ) TArray< const UCampaignDifficultyDefinition* > &Array );

	// Data Definition API
	void Verify( const UObject *WorldContext ) override;

	// Custom Thunks
	DECLARE_FUNCTION( execSortDifficulties );
};