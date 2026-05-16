// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

// UE module definition for Editor only functionality for Project Uplift
class FUpliftEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule( ) override;
	void ShutdownModule( ) override;
};