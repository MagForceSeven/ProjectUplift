// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Subsystems/GameFeatureWorldSubsystem.h"

#include "StrategyWorldSubsystems.generated.h"

//
UCLASS( Abstract )
class CAMPAIGN_API UStrategyWorldSubsystem : public UGameFeatureWorldSubsystem
{
	GENERATED_BODY( )
public:
	// Subsystem API
	[[nodiscard]] bool ShouldCreateSubsystem( UObject *Outer ) const override;
};

//
UCLASS( Abstract )
class CAMPAIGN_API UStrategyTickableWorldSubsystem : public UGameFeatureTickableWorldSubsystem
{
	GENERATED_BODY( )
public:
	// Subsystem API
	[[nodiscard]] bool ShouldCreateSubsystem( UObject *Outer ) const override;
};