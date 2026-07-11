// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/UpliftWorldSubsystems.h"

#include "CampaignWorldSubsystems.generated.h"

// A base class for world subsystems that will only be created in gameplay relevant worlds
UCLASS( Abstract )
class CAMPAIGN_API UCampaignWorldSubsystem : public UUpliftWorldSubsystem
{
	GENERATED_BODY( )
public:
	
protected:
	// Uplift World Subsystem API
	[[nodiscard]] bool DoesSupportWorldType( const FGameplayTag &Tag ) const override;
};

// A base class for tickable world subsystems that will only be created in gameplay relevant worlds
UCLASS( Abstract )
class CAMPAIGN_API UCampaignTickableWorldSubsystem : public UUpliftTickableWorldSubsystem
{
	GENERATED_BODY( )
public:
	
protected:
	// Uplift Tickable World Subsystem API
	[[nodiscard]] bool DoesSupportWorldType( const FGameplayTag &Tag ) const override;
};