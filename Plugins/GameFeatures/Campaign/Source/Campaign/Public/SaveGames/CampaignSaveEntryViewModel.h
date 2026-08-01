// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "MVVMViewModelBase.h"

#include "GameplayTagContainer.h"

#include "CampaignSaveEntryViewModel.generated.h"

enum class ESaveDataLoadResult : uint8;
enum class ESaveGameType : uint8;
struct FEnumeratedSaveGameHeader;

// View model for the data to display for each save game
UCLASS( BlueprintType )
class CAMPAIGN_API UCampaignSaveEntryViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY( )
public:
	// Initialization of the view model for a specific save game
	void Initialize( const FEnumeratedSaveGameHeader &SaveGame );
	
	// The save slot name
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	FString SlotName;

	// The result from trying to load the save
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	ESaveDataLoadResult Result;

	// The time the save was made
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	FDateTime TimeStamp;

	// The primary display name to show to the player
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	FText DisplayName;
	
	// The user facing display name was auto-generated
	UPROPERTY( BlueprintReadOnly )
	bool bAutomatedDisplayName;

	// The content packages that were enabled for that play session
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	TArray< FString > ContentFeatures;

	// The type of save game this header is associated with
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	ESaveGameType SaveType;

	// The type of world that the save was made during
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	FGameplayTag WorldType;

	// Mode specific descriptor for UI display
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	FText Descriptor1;

	// Mode specific descriptor for UI display
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	FText Descriptor2;

	// How to currently display the loading result for this entry
	UFUNCTION( BlueprintCallable, FieldNotify )
	ESlateVisibility ResultVisibility( ) const;

	// How to currently display the literal slot/file name for this entry
	UFUNCTION( BlueprintCallable, FieldNotify )
	ESlateVisibility SlotNameVisibility( ) const;
};