// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "SaveData/SaveDataHeader.h"
#include "SaveGames/UpliftCampaignSaveUtilities.h"

#include "UpliftCampaignSaveHeader.generated.h"

// Game specific header information
UCLASS( )
class CAMPAIGN_API UUpliftCampaignSaveHeader : public USaveDataHeader
{
	GENERATED_BODY( )
public:
	// The type of save game this header is associated with
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	ESaveGameType SaveType = ESaveGameType::User;

	// Mode specific descriptor for UI display
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	FText Descriptor1;

	// Mode specific descriptor for UI display
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	FText Descriptor2;

	// Core Save Header API
	uint32 GetVersion( void ) const override;
	bool IsCompatible( uint32 HeaderVersion ) const override;
	int32 GetFileTypeTag( void ) const override;
};