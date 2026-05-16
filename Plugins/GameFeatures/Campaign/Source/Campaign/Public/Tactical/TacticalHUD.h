// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/CampaignHUD.h"

#include "TacticalHUD.generated.h"

class UTacticalHUDWidget;

// Tactical hud implementation to handle shared details of the primary hud actor
UCLASS( )
class CAMPAIGN_API ATacticalHUD : public ACampaignHUD
{
	GENERATED_BODY( )
public:
	// Actor API
	void BeginPlay( ) override;
	void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

	// HUD API
	void DrawHUD( ) override;
	void NotifyHitBoxBeginCursorOver( FName BoxName ) override;
	void NotifyHitBoxEndCursorOver( FName BoxName ) override;

private:
	// The type of widget to create for this type of HUD
	UPROPERTY( EditDefaultsOnly, Category = "HUD", meta = (AllowAbstract = false) )
	TSubclassOf< UTacticalHUDWidget > HUDWidgetType;

	// The HUD widget that was created
	UPROPERTY( VisibleInstanceOnly, Category = "HUD" )
	TObjectPtr< UTacticalHUDWidget > HUDWidget;
};