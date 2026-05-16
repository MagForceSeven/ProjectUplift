// Copyright Russell Aasland. All Rights Reserved.

#include "Campaign/CampaignGameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CampaignGameInstance)

UCampaignGameInstance* UCampaignGameInstance::Get( const UObject *WorldContext )
{
	const auto World = GEngine->GetWorldFromContextObject( WorldContext, EGetWorldErrorMode::LogAndReturnNull );
	check( World != nullptr );
	
	return Get( World->GetGameInstance( ) );
}

UCampaignGameInstance* UCampaignGameInstance::Get( const UGameInstance *GameInstance )
{
	return GameInstance->GetSubsystem< UCampaignGameInstance >( );
}