// Copyright Russell Aasland. All Rights Reserved.

#include "GameFeatures/Actions/GameFeatureAction_UpliftCampaign.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_UpliftCampaign)

void UGameFeatureAction_UpliftCampaign::OnNewCampaign( UWorld *World ) const
{
	TempWorld = World;

	HandleNewCampaign( World );
	ReceiveNewCampaign( );

	TempWorld = nullptr;
}

void UGameFeatureAction_UpliftCampaign::OnCampaignInProgress( UWorld *World ) const
{
	TempWorld = World;

	HandleCampaignInProgress( World );
	ReceiveCampaignInProgress( );

	TempWorld = nullptr;
}

void UGameFeatureAction_UpliftCampaign::OnStartTactical( UWorld *World ) const
{
	TempWorld = World;

	HandleStartTactical( World );
	ReceiveStartTactical( );

	TempWorld = nullptr;
}