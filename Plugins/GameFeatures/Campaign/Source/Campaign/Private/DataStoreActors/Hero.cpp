// Copyright Russell Aasland. All Rights Reserved.

#include "DataStoreActors/Hero.h"

#include "DataDefinitions/HeroClassDefinition.h"

#include "PersistentDataStore.h"
#include "Tactical/TacticalGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Hero)

bool FHeroSpec::IsValid( ) const
{
	return ClassDefinition.IsValid( ) && !HeroName.IsEmpty( );
}

ADS_Hero* ADS_Hero::SpawnHero( const UObject *WorldContext, const FHeroSpec &Spec )
{
	const auto World = GEngine->GetWorldFromContextObject( WorldContext, EGetWorldErrorMode::LogAndReturnNull );
	if (World == nullptr)
		return nullptr;

	if (!ensureAlways( Spec.IsValid( ) ))
		return nullptr;

	const auto RedirectClass = UPersistentDataStore::GetOverrideClassFor< ADS_Hero >( );

	const auto Character = World->SpawnActorDeferred< ADS_Hero >( RedirectClass, FTransform::Identity );

	Character->ClassDefinition = Spec.ClassDefinition.Get( );
	Character->HeroName = Spec.HeroName;

	Character->OnCreate( Spec );

	Character->FinishSpawning( FTransform::Identity );

	return Character;
}

void ADS_Hero::AppendGameFacts_Implementation( FGameplayTagContainer &OutFacts ) const
{
	OutFacts.AddTag( ClassDefinition->ClassID );
}

TSet< FPrimaryAssetId > ADS_Hero::GatherAssetsForModeBundles_Implementation( const FGameplayTag &Mode ) const
{
	TSet< FPrimaryAssetId > Results;

	if (Mode == ATacticalGameMode::WorldType_Tactical)
	{
		Results.Add( ClassDefinition->GetPrimaryAssetId( ) );
	}

	return Results;
}

UTexture2D * ADS_Hero::GetUIImage_Large( ) const
{
	return ClassDefinition->GetUIImage_Large( );
}

TSoftObjectPtr<UTexture2D> ADS_Hero::GetUIImage_Large_Soft( ) const
{
	return ClassDefinition->GetUIImage_Large_Soft( );
}

#if WITH_EDITOR
FString ADS_Hero::GetCustomActorLabel_Implementation( ) const
{
	auto StringName = HeroName.ToString( );
	StringName.RemoveSpacesInline( );

	return StringName + "-" + ClassDefinition->GetName( );
}

FString ADS_Hero::GetCustomOutlinerFolder_Implementation( ) const
{
	return "Heroes";
}
#endif

#include "DataDefinitions/Exec_DataDefinition.h"
#include "Misc/ExecSF.h"
#include "DataStoreActors/Campaign.h"

using namespace ExecSF_Params;
struct FHeroExecs : public FExecSF
{
	FHeroExecs( )
	{
		AddExec( TEXT( "Uplift.Roster.AddHero" ), TEXT( "Add a new hero to the player roster" ), FExecDelegate::CreateStatic( &FHeroExecs::AddHero ) );
	}

	static void AddHero( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		const UHeroClassDefinition *Class = nullptr;
		FString HeroName;

		const auto Campaign = ADS_Campaign::GetSingleton( World );
		if (Campaign == nullptr)
		{
			Ar.Log( TEXT( "Failed to find Campaign data for 'Uplift.Roster.AddHero'." ) );
			return;
		}

		if (GetParams( Cmd, Class, HeroName ) < 2)
		{
			Ar.Log( TEXT( "Insufficient parameters for 'Uplift.Roster.AddHero'." ) );
			return;
		}

		if (Class == nullptr)
		{
			Ar.Log( TEXT( "Failed to find Class Definition for 'Uplift.Roster.AddHero'." ) );
			return;
		}

		if (HeroName.IsEmpty( ))
		{
			Ar.Log( TEXT( "Failed to specify a Hero Name for 'Uplift.Roster.AddHero'." ) );
			return;
		}

		const FHeroSpec Spec = { Class, FText::FromString( HeroName ) };

		const auto NewHero = ADS_Hero::SpawnHero( World, Spec );
		if (NewHero == nullptr)
		{
			Ar.Logf( TEXT( "Error creating new Hero %s 'Uplift.Roster.AddHero'." ), *HeroName );
			return;
		}

		Campaign->AddToRoster( NewHero );
	}
} GHeroExecs;
