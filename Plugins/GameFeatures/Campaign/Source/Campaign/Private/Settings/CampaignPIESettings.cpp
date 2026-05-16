// Copyright Russell Aasland. All Rights Reserved.

#include "Settings/CampaignPIESettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CampaignPIESettings)

#define LOCTEXT_NAMESPACE "CampaignSettings"

FName UCampaignPIESettings::GetContainerName( ) const
{
	return FName("Project");
}

FName UCampaignPIESettings::GetCategoryName( ) const
{
	return FName("Game");
}

FName UCampaignPIESettings::GetSectionName( ) const
{
	return FName("Campaign PIE Settings");
}

#if WITH_EDITOR
FText UCampaignPIESettings::GetSectionText( ) const
{
	return LOCTEXT( "SectionText", "Campaign PIE Settings" );
}

FText UCampaignPIESettings::GetSectionDescription( ) const
{
	return LOCTEXT( "SectionDescription", "Configuration of Campaign PIE Settings" );
}
#endif

#undef LOCTEXT_NAMESPACE