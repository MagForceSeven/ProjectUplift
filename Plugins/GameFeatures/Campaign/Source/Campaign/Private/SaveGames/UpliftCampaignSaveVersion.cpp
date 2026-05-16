// Copyright Russell Aasland. All Rights Reserved.

#include "SaveGames/UpliftCampaignSaveVersion.h"

// ReSharper disable once CppDeclaratorNeverUsed
static FDevVersionRegistration GRegisterSaveGameVersion( SaveDataVersion, (int32)EUpliftCampaignSaveVersion::Build_Latest, TEXT("SaveGameExample") );

EUpliftCampaignSaveVersion GetCampaignSaveVersion( const FArchive &Ar )
{
	const FCustomVersion *CustomVersion = Ar.GetCustomVersions( ).GetVersion( SaveDataVersion );
	if (CustomVersion == nullptr)
		return EUpliftCampaignSaveVersion::Latest;

	// return the build agnostic view of the version
	return UpliftCampaignSave_StripRTM( (EUpliftCampaignSaveVersion)CustomVersion->Version );
}
