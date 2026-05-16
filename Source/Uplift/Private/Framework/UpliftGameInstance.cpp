// Copyright Russell Aasland. All Rights Reserved.

#include "Framework/UpliftGameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftGameInstance)

UUpliftGameInstance* UUpliftGameInstance::GetInstance( const UObject *WorldContext )
{
	const auto World = GEngine->GetWorldFromContextObject( WorldContext, EGetWorldErrorMode::LogAndReturnNull );
	check( World != nullptr );
	
	return CastChecked< UUpliftGameInstance >( World->GetGameInstance( ) );
}