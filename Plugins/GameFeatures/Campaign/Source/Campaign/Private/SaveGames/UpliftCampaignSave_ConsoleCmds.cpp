// Copyright Russell Aasland. All Rights Reserved.

#include "Misc/ExecSF.h"

#include "SaveGames/UpliftCampaignSaveUtilities.h"
#include "SaveGames/UpliftCampaignSaveSubsystem.h"
#include "SaveGames/CampaignSavesViewModel.h"
#include "SaveGames/CampaignSaveEntryViewModel.h"

#include "DataStoreActors/Campaign.h"

// Engine
#include "EngineUtils.h"

extern FString GetQuickSaveSlotName( const FGuid &CampaignID );

using namespace ExecSF_Params;
struct FSaveGameExecs : public FExecSF
{
	FSaveGameExecs( )
	{
		AddExec( TEXT( "Uplift.SaveGames.SaveToSlot" ), TEXT( "Save the current state of the game" ), FExecDelegate::CreateStatic( &FSaveGameExecs::SaveToSlot ) );
		AddExec( TEXT( "Uplift.SaveGames.LoadSlot" ), TEXT( "Load a game from a specified slot" ), FExecDelegate::CreateStatic( &FSaveGameExecs::LoadSlot ) );

		AddExec( TEXT( "Uplift.SaveGames.AutoSave" ), TEXT( "Save the current state of the game" ), FExecDelegate::CreateStatic( &FSaveGameExecs::AutoSave ) );

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
		AddExec( TEXT( "Uplift.SaveGames.DevSave" ), TEXT( "Save the current state of the game" ), FExecDelegate::CreateStatic( &FSaveGameExecs::DevSave ) );
#endif

		AddExec( TEXT( "Uplift.SaveGames.QuickSave" ), TEXT( "Save the current state of the game" ), FExecDelegate::CreateStatic( &FSaveGameExecs::QuickSave ) );
		AddExec( TEXT( "Uplift.SaveGames.QuickLoad" ), TEXT( "Load most recent (or specified) save game" ), FExecDelegate::CreateStatic( &FSaveGameExecs::QuickLoad ) );

		AddExec( TEXT( "Uplift.SaveGames.ReloadSave"), TEXT( "Load the save that was most recently loaded" ), FExecDelegate::CreateStatic( &FSaveGameExecs::ReloadSave ) );
		AddExec( TEXT( "Uplift.SaveGames.LoadMostRecent"), TEXT( "Load the save that was most recently loaded" ), FExecDelegate::CreateStatic( &FSaveGameExecs::LoadRecent ) );

		AddExec( TEXT( "Uplift.SaveGames.SaveToFile" ), TEXT( "Save the current state of the game to an arbitrary file location" ), FExecDelegate::CreateStatic( &FSaveGameExecs::SaveToFile ) );
		AddExec( TEXT( "Uplift.SaveGames.LoadFile" ), TEXT( "Load a game from an arbitrary file location" ), FExecDelegate::CreateStatic( &FSaveGameExecs::LoadFile ) );

		AddExec( TEXT( "Uplift.SaveGames.UI.RefreshVMs" ), TEXT( "Forcibly refresh the VMs for save game data" ), FExecDelegate::CreateStatic( &FSaveGameExecs::RefreshVMs ) );
	}

	static void SaveToSlot( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		FString SlotName;
		bool Async = false;
		FText DisplayName;
		if (GetParams( Cmd, SlotName, Async, DisplayName ) < 1)
		{
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveGames.SaveToSlot - slot name required." ) ) );
			return;
		}

		if (DisplayName.IsEmpty( ))
			DisplayName = FText::FromString( SlotName );

		if (Async)
			UUpliftCampaignSaveUtilities::SaveToSlot_Async( World, SlotName, 0, ESaveGameType::User, DisplayName );
		else
			// ReSharper disable once CppDeclaratorNeverUsed
			const auto Result = UUpliftCampaignSaveUtilities::SaveToSlot( World, SlotName, 0, ESaveGameType::User, DisplayName );
	}

	static void LoadSlot( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		FString SlotName;
		bool Async = false;
		if (GetParams( Cmd, SlotName, Async ) < 1)
		{
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveGames.SaveToSlot - slot name required." ) ) );
			return;
		}

		const UUpliftCampaignSaveHeader *Header = nullptr;
		const UUpliftCampaignSave *SaveData = nullptr;

		
		if (Async)
			UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot_Async( World, SlotName, 0, { } );
		else
			// ReSharper disable once CppDeclaratorNeverUsed
			const auto Result = UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot( World, SlotName, 0, Header, SaveData );
	}

	static void AutoSave( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		bool Async = false;
		GetParams( Cmd, Async );

		if (Async)
			UUpliftCampaignSaveUtilities::AutoSave_Async( World, "ConsoleCmd", 0, FText::FromString( "Uplift.SaveGames.AutoSave" ) );
		else
			UUpliftCampaignSaveUtilities::AutoSave( World, "ConsoleCmd", 0, FText::FromString( "Uplift.SaveGames.AutoSave" ) );
	}

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	static void DevSave( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		FString SlotName;
		bool Async = false;
		FText DisplayName;
		if (GetParams( Cmd, SlotName, Async, DisplayName ) < 1)
		{
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveGames.DevSave - slot name required." ) ) );
			return;
		}

		if (DisplayName.IsEmpty( ))
			DisplayName = FText::FromString( SlotName );

		if (Async)
			UUpliftCampaignSaveUtilities::DeveloperSave_Async( World, SlotName, 0, DisplayName );
		else
			UUpliftCampaignSaveUtilities::DeveloperSave( World, SlotName, 0, DisplayName );
	}
#endif

	static void QuickSave( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		bool Async = false;
		GetParams( Cmd, Async );

		if (Async)
			UUpliftCampaignSaveUtilities::QuickSave_Async( World, 0 );
		else
			UUpliftCampaignSaveUtilities::QuickSave( World, 0 );
	}

	static void QuickLoad( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		bool Async = false;
		GetParams( Cmd, Async );
		
		const UUpliftCampaignSaveHeader *Header = nullptr;
		const UUpliftCampaignSave *SaveData = nullptr;

		const auto Campaign = ADS_Campaign::GetSingleton( World );
		if (Campaign == nullptr)
			return;
		
		const auto CampaignID = ADS_Campaign::GetCampaignID( World );

		if (Async)
			UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot_Async( World, GetQuickSaveSlotName( CampaignID ), 0, { } );
		else
			// ReSharper disable once CppDeclaratorNeverUsed
			const auto Result = UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot( World, GetQuickSaveSlotName( CampaignID ), 0, Header, SaveData );
	}

	static void ReloadSave( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		bool Async = false;
		GetParams( Cmd, Async );
		
		const auto Subsystem = UUpliftCampaignSaveSubsystem::GetSubsystem( World );
		check( Subsystem != nullptr );
		
		if (Subsystem->LastSaveSlotName.IsEmpty( ))
		{
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveGames.ReloadSave - no saves loaded yet." ) ) );
			return;
		}

		const UUpliftCampaignSaveHeader *Header = nullptr;
		const UUpliftCampaignSave *SaveData = nullptr;

		if (Async)
			UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot_Async( World, Subsystem->LastSaveSlotName, 0, { } );
		else
			// ReSharper disable once CppDeclaratorNeverUsed
			const auto Result = UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot( World, Subsystem->LastSaveSlotName, 0, Header, SaveData );
	}

	static void LoadRecent( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		const auto Subsystem = UUpliftCampaignSaveSubsystem::GetSubsystem( World );
		const auto SavesVM = Subsystem->GetViewModel( );
		const auto MostRecentVM = SavesVM->GetMostRecentEntryVM( );

		const UUpliftCampaignSaveHeader *IgnoredHeader = nullptr;
		const UUpliftCampaignSave *IgnoredSave = nullptr;

		if (MostRecentVM == nullptr)
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveGames.LoadMostRecent - no valid save game available." ) ) );
		else
			(void)UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot( World, MostRecentVM->SlotName, 0, IgnoredHeader, IgnoredSave );
	}

	static void OnFinishSave(const FString &PathName, int32 /*UserIndex*/, bool Success )
	{
		if (!Success && (GEngine->GameViewport != nullptr))
		{
			FConsoleOutputDevice StrOut(GEngine->GameViewport->ViewportConsole);
			StrOut.Logf( TEXT("Failed to write save file: '%s'"), *PathName );
		}
	}

	static void SaveToFile( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		FString PathName;
		bool Async = false;
		if (GetParams( Cmd, PathName, Async ) < 1)
		{
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveGames.SaveToFile - file name required." ) ) );
			return;
		}

		if (Async)
		{
			UUpliftCampaignSaveUtilities::SaveToPath_Async( World, PathName, ESaveGameType::User, FText::FromString( PathName ), FSaveAsyncCallback::CreateStatic( &OnFinishSave ) );
		}
		else
		{
			const auto Result = UUpliftCampaignSaveUtilities::SaveToPath( World, PathName, ESaveGameType::User, FText::FromString( PathName ) );
			if (!Result)
				Ar.Logf( TEXT("Failed to write save file: '%s'"), *PathName );
		}
	}

	static void OnFinishLoad(const FString &PathName, int32 /*UserIndex*/, ESaveDataLoadResult Result, const UUpliftCampaignSaveHeader* /*Header*/, const UUpliftCampaignSave* /*SaveData*/ )
	{
		if ((Result != ESaveDataLoadResult::Success) && (GEngine->GameViewport != nullptr))
		{
			FConsoleOutputDevice StrOut(GEngine->GameViewport->ViewportConsole);
			StrOut.Logf( TEXT("Failed to load save file: '%s'"), *PathName );
		}
	}

	static void LoadFile( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		FString PathName;
		bool Async = false;
		if (GetParams( Cmd, PathName, Async ) < 1)
		{
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveGames.LoadFile - file name required." ) ) );
			return;
		}

		const UUpliftCampaignSaveHeader *Header = nullptr;
		const UUpliftCampaignSave *SaveData = nullptr;
		
		if (Async)
		{
			UUpliftCampaignSaveUtilities::LoadSaveGameFromPath_Async( World, PathName, FLoadAsyncCallback::CreateStatic( &OnFinishLoad ) );
		}
		else
		{
			const auto Result = UUpliftCampaignSaveUtilities::LoadSaveGameFromPath( World, PathName, Header, SaveData );
			if (Result != ESaveDataLoadResult::Success)
				Ar.Logf( TEXT("Failed to load save file: '%s'"), *PathName );
		}
	}
	
	static void RefreshVMs( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		for (const auto VM : TObjectRange< UCampaignSavesViewModel >( ))
			VM->RefreshSaves( );
	}

} GSaveGameExecs;