// Copyright Russell Aasland. All Rights Reserved.

#include "SaveGames/UpliftCampaignSaveSubsystem.h"

#include "SaveGames/UpliftCampaignSaveGame.h"
#include "SaveGames/UpliftCampaignSaveUtilities.h"
#include "SaveGames/CampaignSavesViewModel.h"

#include "GameWorld/UpliftWorldSettings.h"
#include "Strategy/StrategyGameMode.h"
#include "Tactical/TacticalGameMode.h"

#include "SaveData/SaveBlockerBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftCampaignSaveSubsystem)

EExecGameLoading UUpliftCampaignSaveSubsystem::GetSaveGameLoadingType( const UObject *WorldContext )
{
	const auto Subsystem = GetSubsystem( WorldContext );
	if (!ensureAlways( Subsystem != nullptr ))
		return EExecGameLoading::NoData;

	if (Subsystem->SaveGame == nullptr)
	{
		if (!Subsystem->bSaveWasLoaded)
			return EExecGameLoading::NoData;

		if (Subsystem->bSaveWasLevelTransition)
			return EExecGameLoading::LevelTransition;
	}
	else if (Subsystem->SaveGame->SaveType == ESaveGameType::Travel)
	{
		return EExecGameLoading::LevelTransition;
	}

	return EExecGameLoading::SaveGame;
}

void UUpliftCampaignSaveSubsystem::Initialize( FSubsystemCollectionBase &Collection )
{
	Super::Initialize( Collection );

	USaveDataUtilities::OnSaveDataAccessStarted.AddUObject( this, &UUpliftCampaignSaveSubsystem::SaveGameAccessStarted );
	USaveDataUtilities::OnSaveDataAccessEnded.AddUObject( this, &UUpliftCampaignSaveSubsystem::SaveGameAccessEnded );
	UUpliftCampaignSaveUtilities::OnSaveGameCreated.AddUObject( this, &UUpliftCampaignSaveSubsystem::SaveGameCreated );
	UUpliftCampaignSaveUtilities::OnSaveGameDeleted.AddUObject( this, &UUpliftCampaignSaveSubsystem::SaveGameDeleted );

	FWorldDelegates::OnGameInstanceWorldChanged.AddUObject( this, &UUpliftCampaignSaveSubsystem::HandleNewWorld );

	HandleNewWorld( nullptr, nullptr, GetWorld( ) );

	// wait until the world is initialized so that async save actions can be used
	GetWorld( )->OnWorldBeginPlay.AddWeakLambda( this, [ this ]( ) -> void
	{
		ViewModel = NewObject< UCampaignSavesViewModel >( this );
	} );
}

void UUpliftCampaignSaveSubsystem::Deinitialize( )
{
	USaveDataUtilities::OnSaveDataAccessStarted.RemoveAll( this );
	USaveDataUtilities::OnSaveDataAccessEnded.RemoveAll( this );
	UUpliftCampaignSaveUtilities::OnSaveGameCreated.RemoveAll( this );
	UUpliftCampaignSaveUtilities::OnSaveGameDeleted.RemoveAll( this );

	FWorldDelegates::OnStartGameInstance.RemoveAll( this );

	Super::Deinitialize( );
}

void UUpliftCampaignSaveSubsystem::SaveGameAccessStarted( )
{
	OnSaveAccessStarted.Broadcast( );
}

void UUpliftCampaignSaveSubsystem::SaveGameAccessEnded( )
{
	OnSaveAccessEnded.Broadcast( );
}

void UUpliftCampaignSaveSubsystem::SaveGameCreated( int32 UserIndex, const FString &SlotName, const UUpliftCampaignSaveHeader *Header )
{
	OnSaveGameCreated.Broadcast( UserIndex, SlotName, Header );
}

void UUpliftCampaignSaveSubsystem::SaveGameDeleted( int32 UserIndex, const FString &SlotName )
{
	OnSaveGameDeleted.Broadcast( UserIndex, SlotName );
}

void UUpliftCampaignSaveSubsystem::SwitchOnGameLoadingType( const UObject *WorldContext, EExecGameLoading &Exec )
{
	Exec = GetSaveGameLoadingType( WorldContext );
}

void UUpliftCampaignSaveSubsystem::HandleNewWorld( UGameInstance *GameInstance, UWorld *OldWorld, UWorld *NewWorld )
{
	if (NewWorld == nullptr)
		return;

	FGameplayTagContainer GameTypes;
	GameTypes.AddTag( ATacticalGameMode::WorldType_Tactical );
	GameTypes.AddTag( AStrategyGameMode::WorldType_Strategy );

	const auto Settings = Cast< AUpliftWorldSettings >( NewWorld->GetWorldSettings( ) );
	if (Settings->GetWorldType( ).MatchesAny( GameTypes ))
		return;

	NewWorld->GetOnBeginPlayEvent( ).AddUObject( this, &UUpliftCampaignSaveSubsystem::HandleWorldBeginPlay );
}

void UUpliftCampaignSaveSubsystem::HandleWorldBeginPlay( bool bBeginPlay )
{
	ensureAlways( bBeginPlay );
	GetWorld( )->GetOnBeginPlayEvent( ).RemoveAll( this );

	// Prevent campaign saves in maps that are not campaign gameplay
	UUpliftCampaignSaveUtilities::AddSaveGameBlocker( this, FSaveBlocker_Unconditional( "Not Campaign Gameplay" ) );
}

#define LOCTEXT_NAMESPACE "UpliftCampaign_DeveloperSettings"

FName UUpliftCampaignSaveSettings::GetContainerName( ) const
{
	return FName( "Project" );
}

FName UUpliftCampaignSaveSettings::GetCategoryName( ) const
{
	return FName( "Game" );
}

FName UUpliftCampaignSaveSettings::GetSectionName( ) const
{
	return FName( "Game Save Data Settings" );
}

#if WITH_EDITOR
FText UUpliftCampaignSaveSettings::GetSectionText( ) const
{
	return LOCTEXT( "SectionText", "Game Save Data Settings" );
}

FText UUpliftCampaignSaveSettings::GetSectionDescription( ) const
{
	return LOCTEXT( "SectionDescription", "Configuration options regarding Game Save Data" );
}
#endif

#undef LOCTEXT_NAMESPACE