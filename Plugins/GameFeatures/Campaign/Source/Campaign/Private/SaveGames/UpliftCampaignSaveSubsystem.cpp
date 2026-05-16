// Copyright Russell Aasland. All Rights Reserved.

#include "SaveGames/UpliftCampaignSaveSubsystem.h"

#include "SaveData/SaveDataUtilities.h"
#include "SaveGames/UpliftCampaignSaveGame.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UpliftCampaignSaveSubsystem)

UUpliftCampaignSaveSubsystem* UUpliftCampaignSaveSubsystem::Get( const UObject *WorldContext )
{
	if (!ensureAlways( WorldContext != nullptr ))
		return nullptr;
	
	const UWorld *World = GEngine->GetWorldFromContextObject( WorldContext, EGetWorldErrorMode::LogAndReturnNull );
	if (!ensureAlways( World != nullptr ))
		return nullptr;

	return Get( World->GetGameInstance( ) );
}

UUpliftCampaignSaveSubsystem* UUpliftCampaignSaveSubsystem::Get( const UGameInstance *GameInstance )
{
	return GameInstance->GetSubsystem< UUpliftCampaignSaveSubsystem >( );
}

EExecGameLoading UUpliftCampaignSaveSubsystem::GetSaveGameLoadingType( const UObject *WorldContext )
{
	const auto Subsystem = Get( WorldContext );
	if (!ensureAlways( Subsystem != nullptr ))
		return EExecGameLoading::NoData;

	if (Subsystem->SaveGame == nullptr)
	{
		if (!Subsystem->bSaveWasLoaded)
			return EExecGameLoading::NoData;

		if (Subsystem->bSaveWasLevelTransition)
			return EExecGameLoading::LevelTransition;
	}

	if (Subsystem->SaveGame->bTravelSave)
		return EExecGameLoading::LevelTransition;

	return EExecGameLoading::SaveGame;
}

void UUpliftCampaignSaveSubsystem::Initialize( FSubsystemCollectionBase &Collection )
{
	Super::Initialize( Collection );

	USaveDataUtilities::OnSaveDataAccessStarted.AddUObject( this, &UUpliftCampaignSaveSubsystem::SaveGameAccessStarted );
	USaveDataUtilities::OnSaveDataAccessEnded.AddUObject( this, &UUpliftCampaignSaveSubsystem::SaveGameAccessEnded );
}

void UUpliftCampaignSaveSubsystem::Deinitialize( )
{
	USaveDataUtilities::OnSaveDataAccessStarted.RemoveAll( this );
	USaveDataUtilities::OnSaveDataAccessEnded.RemoveAll( this );

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

void UUpliftCampaignSaveSubsystem::SwitchOnGameLoadingType( const UObject *WorldContext, EExecGameLoading &Exec )
{
	Exec = GetSaveGameLoadingType( WorldContext );
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