// Copyright Russell Aasland. All Rights Reserved.

#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "SaveData/SaveDataVersion.h"

#if !defined SF_BUILD_RTM
	#define SF_BUILD_RTM 0
#endif

#if WITH_EDITOR // Editor builds should be able to load development, regardless of branch
	#define SF_SAVES_ALLOW_DEV 1
#elif SF_BUILD_RTM // RTM builds should only load saves from RTM builds
	#define SF_SAVES_ALLOW_DEV 0
#else // Any other build is a developer build
	#define SF_SAVES_ALLOW_DEV 1
#endif

// Versioning data for the save game data
enum class EUpliftCampaignSaveVersion : uint32
{
	Development = 0x00000001,
	
	EmbeddedSlotName, // embedded the slot name into the header
	SaveType, // changed bTravelSave to the ESaveGameType

	// Add all format change versions above this entry
	Current_Plus_One,
	Latest = Current_Plus_One - 1,

	RTM = 0x80000000,

	// The minimum version allowed on either build
	Development_Minimum = SaveType,
	RTM_Minimum = RTM | SaveType,
	
#if !SF_BUILD_RTM
	Build_Minimum = Development_Minimum,
#else
	Build_Minimum = RTM_Minimum,
#endif

	// The current version for this type of build
#if !SF_BUILD_RTM
	Build_Latest = Latest,
#else
	Build_Latest = RTM | Latest,
#endif
};

constexpr uint32 Minimum_Allowed_CL = 3155;

// Utility for stripping the RTM bit from a version so that versioning can be compared across build types
constexpr EUpliftCampaignSaveVersion UpliftCampaignSave_StripRTM( EUpliftCampaignSaveVersion Version ) { return (EUpliftCampaignSaveVersion)((int)Version & ~(int)EUpliftCampaignSaveVersion::RTM); }

static_assert( UpliftCampaignSave_StripRTM(EUpliftCampaignSaveVersion::RTM_Minimum) >= EUpliftCampaignSaveVersion::Development_Minimum, "RTM shouldn't have a lower minimum version than Development!" );
static_assert( EUpliftCampaignSaveVersion::Current_Plus_One < EUpliftCampaignSaveVersion::RTM, "Save Game Version value collision!" ); // just in case

// Utility to get the build agnostic save game version that is associated with an Archive
CAMPAIGN_API EUpliftCampaignSaveVersion GetCampaignSaveVersion( const FArchive &Ar );
