
#pragma once

#include "Modules/ModuleManager.h"

// UE module definition for the runtime functionality of the Campaign Game Feature plugin
class FCampaignModule : public IModuleInterface
{
public:
	// Module Interface API
	void StartupModule() override;
	void ShutdownModule() override;
};
