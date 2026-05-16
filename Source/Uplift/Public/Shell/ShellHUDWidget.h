// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/UpliftHUDWidget.h"

#include "ShellHUDWidget.generated.h"

class UVerticalBox;
class UStarfireFeatureData;

// Shell implementation of a widget to act as the primary HUD widget
UCLASS( Abstract )
class UPLIFT_API UShellHUDWidget : public UUpliftHUDWidget
{
	GENERATED_BODY( )
public:
	// Activatable Widget
	void NativeOnActivated( ) override;
	void NativeOnDeactivated( ) override;

protected:
	// The vertical box that extension widgets should be added to
	UPROPERTY( meta = (BindWidget) )
	TObjectPtr< UVerticalBox > ExtensionButtons;

	// Add buttons from owned gameplay features to the available menu options
	UFUNCTION( BlueprintCallable )
	void AddExtensionWidgets( UVerticalBox *ButtonsContainer );

	// Hook for changes to feature ownership to update the set of extension widgets that are active
	UFUNCTION( )
	void OnOwnedFeatureSetChanged( const UStarfireFeatureData *FeatureData );
};