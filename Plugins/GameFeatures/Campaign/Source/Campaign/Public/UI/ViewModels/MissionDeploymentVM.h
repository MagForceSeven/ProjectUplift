// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "MVVMViewModelBase.h"

#include "MissionDeploymentVM.generated.h"

class UHeroVM;

// View model of the selections and configuration for starting a tactical mission
UCLASS( )
class CAMPAIGN_API UMissionDeploymentVM : public UMVVMViewModelBase
{
	GENERATED_BODY( )
public:
	// UObject API
	void PostInitProperties( ) override;
	
	// Determine if the data is correct to actually deploy on the mission
	UFUNCTION( BlueprintCallable, FieldNotify )
	bool IsDeploymentAllowed( ) const;

	// Gather the collection of possible heroes
	UFUNCTION( BlueprintCallable )
	TArray< UHeroVM* > GetPossibleHeroes( int Index ) const;

	// Update the selected hero for the specified index
	UFUNCTION( BlueprintCallable, meta = (HidePinAssetPicker = "HeroVM") )
	void SetHeroSelection( int Index, UHeroVM *HeroVM );

	// Get the VM's for the current selection of heroes for deployment
	TArray< UHeroVM* > GetSelectedHeroes( ) const { return HeroSelections; }

	// The number of heroes that should be deployed on the mission
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	int DeploymentSize;

protected:
	// The current selection of heroes for deployment
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	TArray< TObjectPtr< UHeroVM > > HeroSelections;

	// All the heroes that are possible selections for deployment
	UPROPERTY( BlueprintReadOnly, FieldNotify )
	TArray< TObjectPtr< UHeroVM > > PossibleHeroes;
};
