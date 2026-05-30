// Copyright Russell Aasland. All Rights Reserved.

#include "DataDefinitions/HeroClassDefinition.h"

#include "AssetValidation/AssetChecks.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeroClassDefinition)

UE_DEFINE_GAMEPLAY_TAG_COMMENT( UHeroClassDefinition::ClassTag_Root,		"Hero.Class",			"Root of hero class identifiers" )
UE_DEFINE_GAMEPLAY_TAG_COMMENT( UHeroClassDefinition::ClassTag_Mason,		"Hero.Class.Mason",		"Hero class specializing in passive defenses"  )
UE_DEFINE_GAMEPLAY_TAG_COMMENT( UHeroClassDefinition::ClassTag_Engineer,	"Hero.Class.Engineer",	"Hero class specializing in active defenses"  )
UE_DEFINE_GAMEPLAY_TAG_COMMENT( UHeroClassDefinition::ClassTag_General,		"Hero.Class.General",	"Hero class specializing in offense"  )

void UHeroClassDefinition::Verify( const UObject *WorldContext )
{
	Super::Verify( WorldContext );

	if (DisplayName.IsEmpty( ))
		AssetChecks::AC_Message( this, TEXT( "No display name configured for hero class." ), WorldContext );

	if (!ClassID.IsValid( ))
		AssetChecks::AC_Message( this, TEXT( "No class id tag configured for hero class." ), WorldContext );
}

#if WITH_EDITOR
const UTexture2D * UHeroClassDefinition::GetThumbnail( ) const
{
	return Icon.LoadSynchronous( );
}
#endif
