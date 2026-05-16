// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/CampaignHUD.h"

#include "StrategyHUD.generated.h"

class UStrategyHUDWidget;

// Strategy hud implementation to handle shared details of the primary hud actor
UCLASS( )
class CAMPAIGN_API AStrategyHUD : public ACampaignHUD
{
	GENERATED_BODY( )
public:
	// Actor API
	void BeginPlay( ) override;
	void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

private:
	// The type of widget to create for this type of HUD
	UPROPERTY( EditDefaultsOnly, Category = "HUD", meta = (AllowAbstract = false) )
	TSubclassOf< UStrategyHUDWidget > HUDWidgetType;

	// The HUD widget that was created
	UPROPERTY( VisibleInstanceOnly, Category = "HUD" )
	TObjectPtr< UStrategyHUDWidget > HUDWidget;
};