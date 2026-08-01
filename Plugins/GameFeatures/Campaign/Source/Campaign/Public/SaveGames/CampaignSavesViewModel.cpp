// Copyright Russell Aasland. All Rights Reserved.

#include "SaveGames/CampaignSavesViewModel.h"

#include "CampaignSaveEntryViewModel.h"
#include "SaveGames/UpliftCampaignSaveUtilities.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CampaignSavesViewModel)

void UCampaignSavesViewModel::PostInitProperties( )
{
	Super::PostInitProperties( );
	
	if (IsTemplate( ))
		return;

	UUpliftCampaignSaveUtilities::EnumerateSaveHeaders_Async( this, 0, FEnumerateHeadersComplete::CreateUObject( this, &UCampaignSavesViewModel::OnHeadersEnumerated ) );
}

const UCampaignSaveEntryViewModel* UCampaignSavesViewModel::FindViewModelBySlotName( const FString &SlotName ) const
{
	for (const auto& VM : ViewModels)
	{
		if (VM->SlotName == SlotName)
			return VM;
	}

	return nullptr;
}

void UCampaignSavesViewModel::RefreshSaves( )
{
	ViewModels.Empty( );
	MostRecent = nullptr;

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( ViewModels );
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( MostRecent );

	UUpliftCampaignSaveUtilities::EnumerateSaveHeaders_Async( this, 0, FEnumerateHeadersComplete::CreateUObject( this, &UCampaignSavesViewModel::OnHeadersEnumerated ) );
}

void UCampaignSavesViewModel::OnHeadersEnumerated( const TArray< FEnumeratedSaveGameHeader > &Headers )
{
	const auto CreateAndAddViewModel = [ this ]( const FEnumeratedSaveGameHeader &Header ) -> void
	{
		const FName VM_Name( "SaveEntryVM_" + Header.SlotName );
		const auto VM = NewObject< UCampaignSaveEntryViewModel >( this, VM_Name );
		VM->Initialize( Header );

		AddEntry( VM );
	};
	
	for (const auto &H : Headers)
		CreateAndAddViewModel( H );

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( ViewModels );
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( MostRecent );
	
	UUpliftCampaignSaveUtilities::OnSaveGameCreated.AddWeakLambda( this, [ this, CreateAndAddViewModel ]( int32 UserIndex, const FString &SlotName, const UUpliftCampaignSaveHeader *Header ) -> void
	{
		check( Header != nullptr );
		
		const auto PreviousRecent = MostRecent;

		FEnumeratedSaveGameHeader NewHeader;
		NewHeader.SlotName = SlotName;
		NewHeader.Header = Header;
		NewHeader.LoadingResult = ESaveDataLoadResult::Success;

		if (const auto VM = FindViewModelBySlotName( SlotName ))
		{
			ViewModels.Remove( VM );

			const_cast< UCampaignSaveEntryViewModel* >( VM )->Initialize( NewHeader );

			AddEntry( VM );
		}
		else
		{
			CreateAndAddViewModel( NewHeader );
		}

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( ViewModels );

		if (PreviousRecent != MostRecent)
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( MostRecent );
	} );
	
	UUpliftCampaignSaveUtilities::OnSaveGameDeleted.AddWeakLambda( this, [ this ]( int32 UserIndex, const FString &SlotName ) -> void
	{
		if (const auto VM = FindViewModelBySlotName( SlotName ))
		{
			ViewModels.Remove( VM );
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( ViewModels );
			
			if (MostRecent == VM)
			{
				MostRecent = (!ViewModels.IsEmpty( ) && (ViewModels[ 0 ]->Result == ESaveDataLoadResult::Success)) ? ViewModels[ 0 ] : nullptr;
				UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( MostRecent );
			}
		}
	} );
}

void UCampaignSavesViewModel::AddEntry( const UCampaignSaveEntryViewModel *ViewModel )
{
	const auto Comparison = [ NewModel = ViewModel ]( const UCampaignSaveEntryViewModel *VM ) -> bool
	{
		const auto bNewIsValid = NewModel->Result == ESaveDataLoadResult::Success;
		const auto bExistingIsValid = VM->Result == ESaveDataLoadResult::Success;

		// Sort invalid saves to the end
		if (bNewIsValid != bExistingIsValid)
			return bNewIsValid;

		// Sort Content Mismatch errors before other types of errors
		if (!bNewIsValid)
		{
			const auto bNewContentError = NewModel->Result == ESaveDataLoadResult::ContentMismatch;
			const auto bExistingContentError = VM->Result == ESaveDataLoadResult::ContentMismatch;
			
			if (bNewContentError != bExistingContentError)
				return bNewContentError;
		}

		// Newer files towards the top
		if (NewModel->TimeStamp > VM->TimeStamp)
			return true;

		if (NewModel->TimeStamp == VM->TimeStamp)
			return NewModel->SlotName < VM->SlotName;

		return false;
	};

	auto bInserted = false;
	for (int idx = 0; idx < ViewModels.Num( ); ++idx)
	{
		const auto VM = ViewModels[ idx ];
		if (Comparison( VM ))
		{
			ViewModels.Insert( ViewModel, idx );
			bInserted = true;
			break;
		}
	}

	if (!bInserted)
		ViewModels.Push( ViewModel );

	if (ViewModels[ 0 ]->Result == ESaveDataLoadResult::Success)
		MostRecent = ViewModels[ 0 ];
}
