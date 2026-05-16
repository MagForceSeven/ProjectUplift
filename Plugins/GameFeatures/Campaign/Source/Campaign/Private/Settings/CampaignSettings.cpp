
#include "Settings/CampaignSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CampaignSettings)

#define LOCTEXT_NAMESPACE "CampaignSettings"

FName UCampaignSettings::GetContainerName( ) const
{
	return FName("Project");
}

FName UCampaignSettings::GetCategoryName( ) const
{
	return FName("Game");
}

FName UCampaignSettings::GetSectionName( ) const
{
	return FName("Campaign Settings");
}

#if WITH_EDITOR
FText UCampaignSettings::GetSectionText( ) const
{
	return LOCTEXT( "SectionText", "Campaign Settings" );
}

FText UCampaignSettings::GetSectionDescription( ) const
{
	return LOCTEXT( "SectionDescription", "Configuration of global Campaign Settings" );
}
#endif

#undef LOCTEXT_NAMESPACE