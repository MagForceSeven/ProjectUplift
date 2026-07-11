// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Campaign/CampaignWorldSubsystems.h"

#include "StrategyWorldSubsystems.generated.h"

// A base class for a world subsystem that will only be activated while in strategy maps
UCLASS( Abstract )
class CAMPAIGN_API UStrategyWorldSubsystem : public UCampaignWorldSubsystem
{
	GENERATED_BODY( )
public:
	
protected:
	// Uplift World Subsystem API
	[[nodiscard]] bool DoesSupportWorldType( const FGameplayTag &Tag ) const override;
};

// A base class for a ticking world subsystem that will only be activated while in strategy maps
UCLASS( Abstract )
class CAMPAIGN_API UStrategyTickableWorldSubsystem : public UCampaignTickableWorldSubsystem
{
	GENERATED_BODY( )
public:
	
protected:
	// Uplift World Subsystem API
	[[nodiscard]] bool DoesSupportWorldType( const FGameplayTag &Tag ) const override;
};