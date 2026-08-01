// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SubsystemNativeAccessors.h"

#include "UpliftCampaignSaveSubsystem.generated.h"

class UUpliftCampaignSaveHeader;
class UUpliftCampaignSave;
class UCampaignSavesViewModel;

UENUM( )
enum class EExecGameLoading : uint8
{
	// No pre-existing game data (PIE or quick-play)
	NoData,
	// Partial pre-existing game data (new game/level travel)
	LevelTransition,
	// Full game data (save game file)
	SaveGame,
};

// A game instance subsystem for use by the save system for various application duration tracking needs
UCLASS( )
class CAMPAIGN_API UUpliftCampaignSaveSubsystem : public UGameInstanceSubsystem, public TSubsystemNativeAccessors< UUpliftCampaignSaveSubsystem >
{
	GENERATED_BODY( )
public:
	// Blueprint accessible hook for async save notification
	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FSaveGameAccessStarted );
	UPROPERTY( BlueprintAssignable )
	FSaveGameAccessStarted OnSaveAccessStarted;

	// Blueprint accessible hook for async save notification
	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FSaveGameAccessEnded );
	UPROPERTY( BlueprintAssignable )
	FSaveGameAccessEnded OnSaveAccessEnded;

	// Blueprint accessible hook for save game creation
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FSaveGameCreated, int32, UserIndex, const FString&, SlotName, const UUpliftCampaignSaveHeader*, Header );
	UPROPERTY( BlueprintAssignable )
	FSaveGameCreated OnSaveGameCreated;

	// Blueprint accessible hook for save game deletion
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FSaveGameDeleted, int32, UserIndex, const FString&, SlotName );
	UPROPERTY( BlueprintAssignable )
	FSaveGameDeleted OnSaveGameDeleted;

	// Accessor to the type of save game loading that happened
	[[nodiscard]] static EExecGameLoading GetSaveGameLoadingType( const UObject *WorldContext );

	// The save data that should be used to populate game objects when the map is loaded
	UPROPERTY( VisibleInstanceOnly )
	TObjectPtr< const UUpliftCampaignSave > SaveGame = nullptr;

	// Save data from the start of a tactical gameplay that can be used to restart
	UPROPERTY( VisibleInstanceOnly )
	TObjectPtr< const UUpliftCampaignSave > TacticalStartCheckpoint;

	// The slot name of the last save that was loaded
	UPROPERTY( VisibleInstanceOnly )
	FString LastSaveSlotName;

	// Was a save loaded on entry into the level
	UPROPERTY( VisibleInstanceOnly )
	bool bSaveWasLoaded = false;

	// Was the loaded save a level transition (as opposed to from disk)
	UPROPERTY( VisibleInstanceOnly )
	bool bSaveWasLevelTransition = false;

	// Accessor to the view model that lists all the saves
	UCampaignSavesViewModel* GetViewModel( void ) { return ViewModel; }

	// Subsystem API
	void Initialize( FSubsystemCollectionBase& Collection ) override;
	void Deinitialize( ) override;

protected:
	// Hooks for the save utility async notifications
	void SaveGameAccessStarted( void );
	void SaveGameAccessEnded( void );
	void SaveGameCreated( int32 UserIndex, const FString &SlotName, const UUpliftCampaignSaveHeader *Header );
	void SaveGameDeleted( int32 UserIndex, const FString &SlotName );

	// Branch the execution on the type of data that is available
	UFUNCTION( BlueprintCallable, Category = "Uplift Save Game", meta = (WorldContext = "WorldContext", ExpandEnumAsExecs = "Exec") )
	static void SwitchOnGameLoadingType( const UObject* WorldContext, EExecGameLoading& Exec );

	// Handle the transition from one world to the next
	UFUNCTION( )
	void HandleNewWorld( UGameInstance *GameInstance, UWorld *OldWorld, UWorld *NewWorld );

	// Handle the start of gameplay in worlds
	UFUNCTION( )
	void HandleWorldBeginPlay( bool bBeginPlay );

	// The view model for all the known campaign save games
	UPROPERTY( VisibleInstanceOnly )
	TObjectPtr< UCampaignSavesViewModel > ViewModel;
};

// Configuration settings for project specific save data settings
UCLASS( Config = "Game", DefaultConfig )
class CAMPAIGN_API UUpliftCampaignSaveSettings : public UDeveloperSettings
{
	GENERATED_BODY( )
public:
	// List of Actor component types that should be serialized when saving an Actor
	// Lots of components shouldn't be included, so we'll opt in to saving
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Config )
	TArray< TSoftClassPtr< UActorComponent > > SavedComponentTypes;

	// List of object types that should not be saved when they exist as sub-objects
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Config )
	TArray< TSoftClassPtr< UObject > > IgnoredSubobjectTypes;

	// Developer Settings API
	FName GetContainerName( ) const override;
	FName GetCategoryName( ) const override;
	FName GetSectionName( ) const override;

#if WITH_EDITOR
	FText GetSectionText( ) const override;
	FText GetSectionDescription( ) const override;
#endif
};