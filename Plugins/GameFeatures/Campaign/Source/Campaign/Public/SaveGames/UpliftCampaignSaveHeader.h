// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "SaveData/SaveDataHeader.h"
#include "SaveGames/UpliftCampaignSaveUtilities.h"

#include "GameplayTagContainer.h"

#include "UpliftCampaignSaveHeader.generated.h"

// Game specific header information
UCLASS( )
class CAMPAIGN_API UUpliftCampaignSaveHeader : public USaveDataHeader
{
	GENERATED_BODY( )
public:
	// The slot name that was used to save this file
	// Platforms might mess with the slot name we generate,
	//   so we'll store it here to make sure it's stable regardless of the
	//   actual filename the slot name is transformed into by the platform
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	FString SlotName;
	
	// The type of save game this header is associated with
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	ESaveGameType SaveType = ESaveGameType::User;

	// The type of world the player was in when they made the save
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	FGameplayTag WorldType;

	// The user facing display name
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	FText UserDisplayName;

	// The user facing display name was auto-generated
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	bool bAutomatedDisplayName;

	// Mode specific descriptor for UI display
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	FText Descriptor1;

	// Mode specific descriptor for UI display
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Save Game" )
	FText Descriptor2;

	// An ID that can correlate saves to a specific playthrough
	UPROPERTY( VisibleInstanceOnly )
	FGuid CampaignID;

	// Core Save Header API
	uint32 GetVersion( void ) const override;
	bool IsCompatible( uint32 HeaderVersion ) const override;
	int32 GetFileTypeTag( void ) const override;
};