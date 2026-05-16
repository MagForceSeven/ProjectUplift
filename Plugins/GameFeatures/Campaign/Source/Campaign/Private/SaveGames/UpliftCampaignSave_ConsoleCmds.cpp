// Copyright Russell Aasland. All Rights Reserved.

#include "Misc/ExecSF.h"

#include "SaveGames/UpliftCampaignSaveUtilities.h"
#include "SaveGames/UpliftCampaignSaveSubsystem.h"

// Engine
#include "EngineUtils.h"

extern FString GetQuickSaveSlotName( void );
extern FString GetQuickSaveDisplayName( void );

using namespace ExecSF_Params;
struct FSaveGameExecs : public FExecSF
{
	FSaveGameExecs( )
	{
		AddExec( TEXT( "Uplift.SaveData.SaveToSlot" ), TEXT( "Save the current state of the game" ), FExecDelegate::CreateStatic( &FSaveGameExecs::SaveToSlot ) );
		AddExec( TEXT( "Uplift.SaveData.LoadSlot" ), TEXT( "Load a game from a specified slot" ), FExecDelegate::CreateStatic( &FSaveGameExecs::LoadSlot ) );

		AddExec( TEXT( "Uplift.SaveData.AutoSave" ), TEXT( "Save the current state of the game" ), FExecDelegate::CreateStatic( &FSaveGameExecs::AutoSave ) );

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
		AddExec( TEXT( "Uplift.SaveData.DevSave" ), TEXT( "Save the current state of the game" ), FExecDelegate::CreateStatic( &FSaveGameExecs::DevSave ) );
#endif

		AddExec( TEXT( "Uplift.SaveData.QuickSave" ), TEXT( "Save the current state of the game" ), FExecDelegate::CreateStatic( &FSaveGameExecs::QuickSave ) );
		AddExec( TEXT( "Uplift.SaveData.QuickLoad" ), TEXT( "Load most recent (or specified) save game" ), FExecDelegate::CreateStatic( &FSaveGameExecs::QuickLoad ) );

		AddExec( TEXT( "Uplift.SaveData.ReloadSave"), TEXT( "Load the save that was most recently loaded" ), FExecDelegate::CreateStatic( &FSaveGameExecs::ReloadSave ) );
		AddExec( TEXT( "Uplift.SaveData.LoadMostRecent"), TEXT( "Load the save that was most recently loaded" ), FExecDelegate::CreateStatic( &FSaveGameExecs::LoadRecent ) );

		AddExec( TEXT( "Uplift.SaveData.SaveToFile" ), TEXT( "Save the current state of the game to an arbitrary file location" ), FExecDelegate::CreateStatic( &FSaveGameExecs::SaveToFile ) );
		AddExec( TEXT( "Uplift.SaveData.LoadFile" ), TEXT( "Load a game from an arbitrary file location" ), FExecDelegate::CreateStatic( &FSaveGameExecs::LoadFile ) );
	}

	static void SaveToSlot( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		FString SlotName;
		bool Async = false;
		if (GetParams( Cmd, SlotName, Async ) < 1)
		{
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveData.SaveToSlot - slot name required." ) ) );
			return;
		}

		if (Async)
			UUpliftCampaignSaveUtilities::SaveToSlot_Async( World, SlotName, 0, ESaveGameType::User );
		else
			// ReSharper disable once CppDeclaratorNeverUsed
			const auto Result = UUpliftCampaignSaveUtilities::SaveToSlot( World, SlotName, 0, ESaveGameType::User );
	}

	static void LoadSlot( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		FString SlotName;
		bool Async = false;
		if (GetParams( Cmd, SlotName, Async ) < 1)
		{
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveData.SaveToSlot - slot name required." ) ) );
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
			UUpliftCampaignSaveUtilities::AutoSave_Async( World, 0 );
		else
			UUpliftCampaignSaveUtilities::AutoSave( World, 0 );
	}

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	static void DevSave( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		FString SlotName;
		bool Async = false;
		if (GetParams( Cmd, SlotName, Async ) < 1)
		{
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveData.DevSave - slot name required." ) ) );
			return;
		}

		if (Async)
			UUpliftCampaignSaveUtilities::DeveloperSave_Async( World, SlotName, 0 );
		else
			UUpliftCampaignSaveUtilities::DeveloperSave( World, SlotName, 0 );
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

		if (Async)
			UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot_Async( World, GetQuickSaveSlotName( ), 0, { } );
		else
			// ReSharper disable once CppDeclaratorNeverUsed
			const auto Result = UUpliftCampaignSaveUtilities::LoadSaveGameFromSlot( World, GetQuickSaveSlotName( ), 0, Header, SaveData );
	}

	static void ReloadSave( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		bool Async = false;
		GetParams( Cmd, Async );
		
		const auto Subsystem = UUpliftCampaignSaveSubsystem::Get( World );
		check( Subsystem != nullptr );
		
		if (Subsystem->LastSaveSlotName.IsEmpty( ))
		{
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveData.ReloadSave - no saves loaded yet." ) ) );
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
		bool Async = false;
		GetParams( Cmd, Async );
		
		const UUpliftCampaignSaveHeader *Header = nullptr;
		const UUpliftCampaignSave *SaveData = nullptr;
		FString SlotName;

		if (Async)
			UUpliftCampaignSaveUtilities::LoadMostRecentSave_Async( World, 0, { } );
		else
			// ReSharper disable once CppDeclaratorNeverUsed
			const auto Result = UUpliftCampaignSaveUtilities::LoadMostRecentSave( World, 0, SlotName, Header, SaveData );
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
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveData.SaveToFile - file name required." ) ) );
			return;
		}

		if (Async)
		{
			UUpliftCampaignSaveUtilities::SaveToPath_Async( World, PathName, ESaveGameType::User, { }, FSaveAsyncCallback::CreateStatic( &OnFinishSave ) );
		}
		else
		{
			const auto Result = UUpliftCampaignSaveUtilities::SaveToPath( World, PathName, ESaveGameType::User );
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
			Ar.Log( FString::Printf( TEXT( "Uplift.SaveData.LoadFile - file name required." ) ) );
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

} GSaveGameExecs;