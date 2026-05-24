// Copyright Russell Aasland. All Rights Reserved.

#include "SaveGames/UpliftCampaignSaveHeader.h"

#include "SaveGames/UpliftCampaignSaveGame.h"
#include "SaveGames/UpliftCampaignSaveUtilities.h"
#include "SaveGames/UpliftCampaignSaveVersion.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftCampaignSaveHeader)

int32 UUpliftCampaignSaveHeader::GetFileTypeTag( void ) const
{
	// ReSharper disable once CppMultiCharacterLiteral
	return 'UPLC';
}

UUpliftCampaignSaveHeader* UUpliftCampaignSaveUtilities::CreateSaveGameHeader( const UUpliftCampaignSave *SaveGameData, ESaveGameType SaveType, const FString &DisplayName )
{
	if (!ensureAlways( SaveGameData != nullptr ))
		return nullptr;

	const auto Header = NewObject< UUpliftCampaignSaveHeader >( GetTransientPackage( ) );

	Header->FillCoreData( SaveGameData, DisplayName );

	Header->SaveType = SaveType;
	Header->Descriptor1 = SaveGameData->Descriptor1;
	Header->Descriptor2 = SaveGameData->Descriptor2;
	Header->CampaignID = SaveGameData->CampaignID;

	return Header;
}

uint32 UUpliftCampaignSaveHeader::GetVersion( ) const
{
	return (uint32)EUpliftCampaignSaveVersion::Build_Latest;
}

static TAutoConsoleVariable< int > CVar_MinAllowedHeaderVersion( TEXT( "Uplift.SaveGames.SetMinAllowedSaveHeaderVersion" ),
	(int)EUpliftCampaignSaveVersion::Build_Minimum, TEXT( "Change the minimum supported version for loading save games" ), ECVF_Cheat );
static TAutoConsoleVariable< int > CVar_MaxAllowedHeaderVersion( TEXT( "Uplift.SaveGames.SetMaxAllowedSaveHeaderVersion" ),
	(int)EUpliftCampaignSaveVersion::Build_Latest, TEXT( "Change the maximum supported version for loading save games" ), ECVF_Cheat );

bool UUpliftCampaignSaveHeader::IsCompatible( uint32 HeaderVersion ) const
{
#if !SF_SAVES_ALLOW_DEV
	// If this version isn't RTM, we can't load it
	if ((HeaderVersion & (uint32)EUpliftCampaignSaveVersion::RTM) == 0)
		return false;
#endif

	// Convert both the version and minimum version to build-agnostic values
	const auto Version = UpliftCampaignSave_StripRTM( (EUpliftCampaignSaveVersion)HeaderVersion );

	const auto MinimumCompatibleVersion = (EUpliftCampaignSaveVersion)CVar_MinAllowedHeaderVersion.GetValueOnAnyThread( );
	const auto BuildMinimum = UpliftCampaignSave_StripRTM( MinimumCompatibleVersion );

	// Below the minimum allowed for this build
	if (Version < BuildMinimum)
		return false;

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	const auto MaximumCompatibleVersion = (EUpliftCampaignSaveVersion)CVar_MaxAllowedHeaderVersion.GetValueOnAnyThread( );
	const auto BuildMaximum = UpliftCampaignSave_StripRTM( MaximumCompatibleVersion );
	if (Version > BuildMaximum)
		return false;
#endif

	// Above the maximum known to this build
	if (Version > EUpliftCampaignSaveVersion::Latest)
		return false;

	return true;
}
