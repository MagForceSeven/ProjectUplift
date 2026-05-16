// Copyright Russell Aasland. All Rights Reserved.

#include "Shell/ShellHUD.h"

#include "Shell/ShellHUDWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ShellHUD)

void AShellHUD::BeginPlay( )
{
	Super::BeginPlay( );

	if (HUDWidgetType != nullptr)
		HUDWidget = CastChecked< UShellHUDWidget >( CreateHUDWidget( HUDWidgetType ) );
}

void AShellHUD::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	HUDWidget = nullptr;
	
	Super::EndPlay( EndPlayReason );
}
