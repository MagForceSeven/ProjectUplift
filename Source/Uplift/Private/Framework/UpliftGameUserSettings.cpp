// Copyright Russell Aasland. All Rights Reserved.

#include "Framework/UpliftGameUserSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftGameUserSettings)

UUpliftGameUserSettings* UUpliftGameUserSettings::Get( )
{
	return CastChecked< UUpliftGameUserSettings >( GEngine->GetGameUserSettings( ) );
}