// Copyright Russell Aasland. All Rights Reserved.

#include "SaveGames/CampaignSavesVMResolver.h"

#include "SaveGames/CampaignSavesViewModel.h"
#include "SaveGames/UpliftCampaignSaveSubsystem.h"

// UMG
#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CampaignSavesVMResolver)

UObject* UCampaignSavesVMResolver::CreateInstance( const UClass *ExpectedType, const UUserWidget *UserWidget, const UMVVMView *View ) const
{
	if (!ensureAlways( ExpectedType->IsChildOf< UCampaignSavesViewModel >( ) ))
		return nullptr;
	
	const auto Subsystem = UUpliftCampaignSaveSubsystem::GetSubsystem( UserWidget );
	check( Subsystem != nullptr );

	return Subsystem->GetViewModel( );
}

#if WITH_EDITOR
bool UCampaignSavesVMResolver::DoesSupportViewModelClass( const UClass *Class ) const
{
	return Class->IsChildOf< UCampaignSavesViewModel >( );
}
#endif