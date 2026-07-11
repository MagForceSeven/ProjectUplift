// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Campaign/CampaignWorldSubsystems.h"

#include "TacticalWorldSubsystems.generated.h"

// A base class for a world subsystem that will only be created in maps supporting tactical gameplay
UCLASS( Abstract )
class CAMPAIGN_API UTacticalWorldSubsystem : public UCampaignWorldSubsystem
{
	GENERATED_BODY( )
public:
	
protected:
	// Uplift World Subsystem API
	[[nodiscard]] bool DoesSupportWorldType( const FGameplayTag &Tag ) const override;
};

// A base class for a ticking world subsystem that will only be created in maps supporting tactical gameplay
UCLASS( Abstract )
class CAMPAIGN_API UTacticalTickableWorldSubsystem : public UCampaignTickableWorldSubsystem
{
	GENERATED_BODY( )
public:
	
protected:
	// Uplift Tickable World Subsystem API
	[[nodiscard]] bool DoesSupportWorldType( const FGameplayTag &Tag ) const override;
};