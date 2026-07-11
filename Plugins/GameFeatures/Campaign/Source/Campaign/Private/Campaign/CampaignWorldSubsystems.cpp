// Copyright Russell Aasland. All Rights Reserved.

#include "Campaign/CampaignWorldSubsystems.h"

#include "GameWorld/CampaignGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CampaignWorldSubsystems)

bool UCampaignWorldSubsystem::DoesSupportWorldType( const FGameplayTag &Tag ) const
{
	if (!Super::DoesSupportWorldType( Tag ))
		return false;

	return Tag.MatchesTag( ACampaignGameMode::WorldType_Gameplay );
}

bool UCampaignTickableWorldSubsystem::DoesSupportWorldType( const FGameplayTag &Tag ) const
{
	if (!Super::DoesSupportWorldType( Tag ))
		return false;

	return Tag.MatchesTag( ACampaignGameMode::WorldType_Gameplay );
}
