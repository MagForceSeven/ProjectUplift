// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

// UE module definition for Developer only build functionality for Project Uplift
class FUpliftDeveloperModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule( ) override;
	void ShutdownModule( ) override;
};