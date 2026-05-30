// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameFeatureAction.h"

#include "GameFeatureAction_UpliftCampaign.generated.h"

// Custom action that can be used by Features to add gameplay to the model at certain times
// 'Const' to force blueprint implementations to have immediate completions
UCLASS( Abstract, Blueprintable, DisplayName = "Uplift Campaign Hooks", Const )
class CAMPAIGN_API UGameFeatureAction_UpliftCampaign : public UGameFeatureAction
{
	GENERATED_BODY( )
public:
	// Update the gameplay model when starting a new campaign (pre-World BeginPlay)
	void OnNewCampaign( UWorld *World ) const;

	// Update the gameplay model of a campaign that is already in progress (post-World BeginPlay)
	void OnCampaignInProgress( UWorld *World ) const;

	// Update the gameplay model at the start of tactical (pre-World BeginPlay)
	// Called at the start of *every* tactical session when the feature is active
	void OnStartTactical( UWorld *World ) const;

	// UObject API
	UWorld* GetWorld( ) const override { return TempWorld.Get( ); }

protected:
	// Implementation hook for modifying the gameplay model when starting a new campaign (pre-World BeginPlay)
	virtual void HandleNewCampaign( UWorld *World ) const { }

	// Implementation hook for modifying the gameplay model when starting a new campaign (pre-World BeginPlay)
	UFUNCTION( BlueprintImplementableEvent, DisplayName = "On New Campaign" )
	void ReceiveNewCampaign( ) const;

	// Implementation hook for modifying the gameplay model of an in-progress campaign (post-World BeginPlay)
	virtual void HandleCampaignInProgress( UWorld *World ) const { }

	// Implementation hook for modifying the gameplay model of an in-progress campaign (post-World BeginPlay)
	UFUNCTION( BlueprintImplementableEvent, DisplayName = "On Campaign In Progress" )
	void ReceiveCampaignInProgress( ) const;

	// Implementation hook for modifying the gameplay model at the start of tactical (pre-World BeginPlay)
	// Called at the start of *every* tactical session when the feature is active
	virtual void HandleStartTactical( UWorld *World ) const { }

	// Implementation hook for modifying the gameplay model at the start of tactical (pre-World BeginPlay)
	// Called at the start of *every* tactical session when the feature is active
	UFUNCTION( BlueprintImplementableEvent, DisplayName = "On Start Tactical" )
	void ReceiveStartTactical( ) const;

private:
	// A temporary world reference to support the GetWorld/WorldContext functionality
	mutable TWeakObjectPtr< UWorld > TempWorld;
};