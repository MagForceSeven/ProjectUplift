// Copyright Russell Aasland. All Rights Reserved.

#include "UI/ViewModels/HeroClassVM.h"

#include "DataStoreActors/Hero.h"
#include "DataDefinitions/HeroClassDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeroClassVM)

void UHeroClassVM::HandleOnCreate( AActor *Actor )
{
	Super::HandleOnCreate( Actor );

	if (const auto Hero = Cast< ADS_Hero >( Actor ) )
	{
		InitFromObject( Hero->GetClassDefinition( ) );
	}
}