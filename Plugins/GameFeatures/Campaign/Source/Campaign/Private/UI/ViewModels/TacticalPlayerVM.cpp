// Copyright Russell Aasland. All Rights Reserved.

#include "UI/ViewModels/TacticalPlayerVM.h"

#include "ActorViewModels/ActorVMUtilities.h"
#include "UI/ViewModels/HeroVM.h"

#include "DataStoreActors/BattleData.h"
#include "DataStoreActors/Hero.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TacticalPlayerVM)

void UTacticalPlayerVM::HandleOnCreate( AActor *Actor )
{
	Super::HandleOnCreate( Actor );

	const auto BattleData = ADS_BattleData::GetSingleton( Actor );

	for (const auto &Hero : BattleData->Squad)
		HeroDisplayVMs.Push( UActorVMUtilities::FindOrCreateVM< UHeroVM >( Hero ) );

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED( HeroDisplayVMs );
}
