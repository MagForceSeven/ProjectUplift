// Copyright Russell Aasland. All Rights Reserved.

#include "SaveGames/CampaignSaveEntryViewModel.h"

#include "SaveGames/CampaignSavesViewModel.h"

#include "SaveGames/UpliftCampaignSaveHeader.h"
#include "SaveGames/UpliftCampaignSaveUtilities.h"

// UMG
#include "Components/SlateWrapperTypes.h"

// Core
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CampaignSaveEntryViewModel)

void UCampaignSaveEntryViewModel::Initialize( const FEnumeratedSaveGameHeader &SaveGame )
{
	UE_MVVM_SET_PROPERTY_VALUE( SlotName, SaveGame.SlotName );
	UE_MVVM_SET_PROPERTY_VALUE( Result, SaveGame.LoadingResult );
	UE_MVVM_SET_PROPERTY_VALUE( TimeStamp, SaveGame.Header->TimeStamp );
	UE_MVVM_SET_PROPERTY_VALUE( ContentFeatures, SaveGame.Header->ContentFeatures );
	UE_MVVM_SET_PROPERTY_VALUE( SaveType, SaveGame.Header->SaveType );
	UE_MVVM_SET_PROPERTY_VALUE( WorldType, SaveGame.Header->WorldType );
	UE_MVVM_SET_PROPERTY_VALUE( Descriptor1, SaveGame.Header->Descriptor1 );
	UE_MVVM_SET_PROPERTY_VALUE( Descriptor2, SaveGame.Header->Descriptor2 );
	bAutomatedDisplayName = SaveGame.Header->bAutomatedDisplayName;

	// TODO: language mismatch display name handling
	const auto CurrentLanguage = FInternationalization::Get( ).GetCurrentLanguage( )->GetName( );
	if (SaveGame.Header->Language == CurrentLanguage)
		UE_MVVM_SET_PROPERTY_VALUE( DisplayName, SaveGame.Header->UserDisplayName );
	else
		UE_MVVM_SET_PROPERTY_VALUE( DisplayName, FText::FromString( SaveGame.Header->DisplayName ) );

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( ResultVisibility );
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( SlotNameVisibility );
}

ESlateVisibility UCampaignSaveEntryViewModel::ResultVisibility( ) const
{
	if (Result == ESaveDataLoadResult::ContentMismatch)
		return ESlateVisibility::HitTestInvisible;

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	if (Result != ESaveDataLoadResult::Success)
		return ESlateVisibility::HitTestInvisible;
#endif

	return ESlateVisibility::Collapsed;
}

static TAutoConsoleVariable< bool > CVar_SaveUIShowSlotName( TEXT( "Uplift.SaveGames.UI.ShowSlotName" ),
	false, TEXT( "Show debug elements for showing the slot name" ),
	FConsoleVariableDelegate::CreateLambda( [ ]( IConsoleVariable* ) -> void
	{
		// Poke all the entry view models to update the visibility of any slot name element they might have
		for (const auto VM : TObjectRange< UCampaignSaveEntryViewModel >( ))
			VM->BroadcastFieldValueChanged( UCampaignSaveEntryViewModel::FFieldNotificationClassDescriptor::SlotNameVisibility );
	} ), ECVF_Cheat );

ESlateVisibility UCampaignSaveEntryViewModel::SlotNameVisibility( ) const
{
#if !UE_BUILD_SHIPPING
	if (CVar_SaveUIShowSlotName.GetValueOnAnyThread( ))
		return ESlateVisibility::HitTestInvisible;
#endif
	
	return ESlateVisibility::Collapsed;
}
