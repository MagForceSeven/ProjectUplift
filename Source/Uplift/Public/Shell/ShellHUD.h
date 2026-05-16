// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/UpliftHUD.h"

#include "ShellHUD.generated.h"

class UShellHUDWidget;
// Shell hud implementation to handle shared details of the primary hud actor
UCLASS( )
class UPLIFT_API AShellHUD : public AUpliftHUD
{
	GENERATED_BODY( )
public:
	// Actor API
	void BeginPlay( ) override;
	void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

private:
	// The type of widget to create for this type of HUD
	UPROPERTY( EditDefaultsOnly, Category = "HUD", meta = (AllowAbstract = false) )
	TSubclassOf< UShellHUDWidget > HUDWidgetType;

	// The HUD widget that was created
	UPROPERTY( VisibleInstanceOnly, Category = "HUD" )
	TObjectPtr< UShellHUDWidget > HUDWidget;
};