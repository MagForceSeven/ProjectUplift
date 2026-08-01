// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "View/MVVMViewModelContextResolver.h"

#include "CampaignSavesVMResolver.generated.h"

// Resolver that can be used to bind to the VM listing campaign saves
UCLASS( DisplayName = "Campaign Saves VM Resolver" )
class CAMPAIGN_API UCampaignSavesVMResolver : public UMVVMViewModelContextResolver
{
	GENERATED_BODY( )
public:
	// MVVM View Model Context Resolver API
	UObject* CreateInstance( const UClass *ExpectedType, const UUserWidget *UserWidget, const UMVVMView *View ) const override;

#if WITH_EDITOR
	bool DoesSupportViewModelClass( const UClass* Class ) const override;
#endif
};