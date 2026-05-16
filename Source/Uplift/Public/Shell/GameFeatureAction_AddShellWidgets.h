// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameFeatureAction.h"

#include "GameFeatureAction_AddShellWidgets.generated.h"

class UShellExtensionWidget;

// Feature action that can be used to inject additional controls into the main menu
UCLASS( DisplayName = "Add Shell Widgets" )
class UPLIFT_API UGameFeatureAction_AddShellWidgets : public UGameFeatureAction
{
	GENERATED_BODY( )
public:
	// The sort order to include the new widgets (relative to widgets from actions in other features)
	UPROPERTY( EditDefaultsOnly )
	int Priority = 0;

	// The types of buttons that should be added to the main menu
	UPROPERTY( EditDefaultsOnly, meta = (AssetBundles = "MainMenu", DisplayThumbnail = false) )
	TArray< TSoftClassPtr< UShellExtensionWidget > > AdditionalButtons;
};