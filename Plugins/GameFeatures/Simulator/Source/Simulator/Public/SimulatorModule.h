// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

// UE module definition for the runtime functionality of the Simulator Game Feature plugin
class FSimulatorModule : public IModuleInterface
{
public:
	// Module Interface API
	void StartupModule() override;
	void ShutdownModule() override;
};
