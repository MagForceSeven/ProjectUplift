// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "MVVMViewModelBase.h"

#include "CampaignSavesViewModel.generated.h"

class UCampaignSaveEntryViewModel;

struct FEnumeratedSaveGameHeader;

// View model for the collection of all save game meta data
UCLASS( BlueprintType )
class CAMPAIGN_API UCampaignSavesViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY( )
public:
	// UObject API
	void PostInitProperties( ) override;
	
	// Accessor to the entry view models in this collection
	TArray< const UCampaignSaveEntryViewModel* > GetViewModels( void ) const { return ViewModels; }
	
	// Accessor to the most recent valid save game
	const UCampaignSaveEntryViewModel* GetMostRecentEntryVM( void ) const { return MostRecent; }
	
	// Find a view model entry for a specific slot name (nullptr if none exists)
	UFUNCTION( BlueprintCallable, BlueprintPure = false )
	const UCampaignSaveEntryViewModel* FindViewModelBySlotName( const FString &SlotName ) const;

	// Forcibly refresh the view model to reflect whatever is on disk
	void RefreshSaves( void );

protected:
	// Collection of view models that represent all the known save files
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	TArray< TObjectPtr< const UCampaignSaveEntryViewModel > > ViewModels;

	// Cached reference to the most recent view model
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	TObjectPtr< const UCampaignSaveEntryViewModel > MostRecent;

private:
	// Handler for an initial enumeration of save games
	void OnHeadersEnumerated( const TArray< FEnumeratedSaveGameHeader > &Headers );

	// Insert a new VM into the list of Save Game View Models sorted against existing view models
	void AddEntry( const UCampaignSaveEntryViewModel *ViewModel );
};