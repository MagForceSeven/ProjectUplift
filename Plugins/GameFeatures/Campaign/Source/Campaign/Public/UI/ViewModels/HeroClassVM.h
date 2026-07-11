// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "ViewModels/DisplayParamsVM.h"

#include "HeroClassVM.generated.h"

// View of the hero class information
UCLASS( )
class CAMPAIGN_API UHeroClassVM : public UDisplayParamsVM
{
	GENERATED_BODY( )
public:

protected:
	// Actor VM Base API
	void HandleOnCreate( AActor *Actor ) override;
};