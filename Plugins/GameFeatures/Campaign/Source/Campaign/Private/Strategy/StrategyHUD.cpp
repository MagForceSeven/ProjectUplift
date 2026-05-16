// Copyright Russell Aasland. All Rights Reserved.

#include "Strategy/StrategyHUD.h"

#include "Strategy/StrategyHUDWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StrategyHUD)

void AStrategyHUD::BeginPlay( )
{
	Super::BeginPlay( );

	if (HUDWidgetType != nullptr)
		HUDWidget = CastChecked< UStrategyHUDWidget >( CreateHUDWidget( HUDWidgetType ) );
}

void AStrategyHUD::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	HUDWidget = nullptr;
	
	Super::EndPlay( EndPlayReason );
}
