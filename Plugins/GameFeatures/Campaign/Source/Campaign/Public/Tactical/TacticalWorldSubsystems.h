// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Subsystems/GameFeatureWorldSubsystem.h"

#include "TacticalWorldSubsystems.generated.h"

//
UCLASS( Abstract )
class CAMPAIGN_API UTacticalWorldSubsystem : public UGameFeatureWorldSubsystem
{
	GENERATED_BODY( )
public:
	// Subsystem API
	[[nodiscard]] bool ShouldCreateSubsystem( UObject *Outer ) const override;
};

//
UCLASS( Abstract )
class CAMPAIGN_API UTacticalTickableWorldSubsystem : public UGameFeatureTickableWorldSubsystem
{
	GENERATED_BODY( )
public:
	// Subsystem API
	[[nodiscard]] bool ShouldCreateSubsystem( UObject *Outer ) const override;
};