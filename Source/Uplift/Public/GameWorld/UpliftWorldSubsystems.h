// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Subsystems/GameFeatureWorldSubsystem.h"

#include "GameplayTagContainer.h"

#include "UpliftWorldSubsystems.generated.h"

// A base class for project world subsystems that defaults to Game & PIE Worlds but also provides another method
// of checking compatibility by comparing against a tag that is part of the world settings for Uplift maps
UCLASS( Abstract )
class UPLIFT_API UUpliftWorldSubsystem : public UGameFeatureWorldSubsystem
{
	GENERATED_BODY( )
public:
	// Subsystem API
	[[nodiscard]] bool ShouldCreateSubsystem( UObject *Outer ) const override;
	
protected:
	// World Subsystem API
	[[nodiscard]] bool DoesSupportWorldType( const EWorldType::Type WorldType ) const override
	{
		return (WorldType == EWorldType::Game) || (WorldType == EWorldType::PIE);
	}

	// Another hook for derived types to check if they support the map being loaded into
	[[nodiscard]] virtual bool DoesSupportWorldType( const FGameplayTag &WorldType ) const { return true; }
};

// A base class for ticking project world subsystems that defaults to Game & PIE Worlds but also provides another method
// of checking compatibility by comparing against a tag that is part of the world settings for Uplift maps
UCLASS( Abstract )
class UPLIFT_API UUpliftTickableWorldSubsystem : public UGameFeatureTickableWorldSubsystem
{
	GENERATED_BODY( )
public:
	// Subsystem API
	[[nodiscard]] bool ShouldCreateSubsystem( UObject *Outer ) const override;
	
protected:
	// World Subsystem API
	[[nodiscard]] bool DoesSupportWorldType( const EWorldType::Type WorldType ) const override
	{
		return (WorldType == EWorldType::Game) || (WorldType == EWorldType::PIE);
	}

	// Another hook for derived types to check if they support the map being loaded into
	[[nodiscard]] virtual bool DoesSupportWorldType( const FGameplayTag &WorldType ) const { return true; }
};