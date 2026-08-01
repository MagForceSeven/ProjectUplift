// Copyright Russell Aasland. All Rights Reserved.

#include "Strategy/StrategyGameMode.h"

#include "Strategy/StrategyGameState.h"
#include "Strategy/StrategyPlayerController.h"
#include "Strategy/StrategyPlayerState.h"
#include "Strategy/StrategyHUD.h"
#include "Strategy/StrategyModeScopeCollection.h"

#include "CampaignBundles.h"
#include "Settings/CampaignSettings.h"
#include "Settings/CampaignPIESettings.h"

#include "SaveGames/UpliftCampaignSaveSubsystem.h"
#include "SaveGames/UpliftCampaignSaveUtilities.h"

#include "PersistentDataStore.h"
#include "DataStoreActors/BattleData.h"
#include "DataStoreActors/Campaign.h"

#include "GameFeatures/StarfireFeatureData.h"
#include "GameFeatures/Actions/GameFeatureAction_UpliftCampaign.h"

#include "Components/GameModeViewModels.h"
#include "UI/ViewModels/RosterVM.h"

#include "Messenger/Messenger.h"

// Engine
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StrategyGameMode)

UE_DEFINE_GAMEPLAY_TAG_COMMENT( AStrategyGameMode::WorldType_Strategy, "World.Type.Gameplay.Strategy", "A world supporting the strategy meta-game progression." );

AStrategyGameMode::AStrategyGameMode( )
{
	GameStateClass = AStrategyGameState::StaticClass( );
	PlayerControllerClass = AStrategyPlayerController::StaticClass( );
	PlayerStateClass = AStrategyPlayerState::StaticClass( );
	HUDClass = AStrategyHUD::StaticClass( );

	GameModeBundles.Push( CampaignBundles::Strategy );

	DefaultViewModels->DefaultViewModels.Push( URosterVM::StaticClass( ) );
}

void AStrategyGameMode::LaunchNewGame( const UObject *WorldContext )
{
	const auto Settings = GetDefault< UCampaignSettings >( );
	const auto StrategyPath = Settings->StrategyLevel.GetLongPackageFName( );
	
	UGameplayStatics::OpenLevel( WorldContext, StrategyPath );
}

void AStrategyGameMode::InitializeCommon( )
{
	Super::InitializeCommon( );

	const auto ScopeCollection = GetWorld( )->SpawnActor< AStrategyModeScopeCollection >( );
}

void AStrategyGameMode::TransitionIntoMode( )
{
	Super::TransitionIntoMode( );
}

void AStrategyGameMode::GameModeReady( )
{
	Super::GameModeReady( );

	if (UUpliftCampaignSaveSubsystem::GetSaveGameLoadingType( this ) == EExecGameLoading::LevelTransition)
	{
		const auto DataStore = UPersistentDataStore::GetSubsystem( this );
		check( DataStore != nullptr );

		const auto BattleData = DataStore->GetSingleton< ADS_BattleData >( );

		UStarfireMessenger::GetSubsystem( this )->Broadcast< FMessage_BattleDataCleanup >( { .BattleData=BattleData } );
		
		BattleData->Destroy( );
	}

	UStarfireMessenger::GetSubsystem( this )->Broadcast< FMessage_StrategyModeReady >( );

	if (UUpliftCampaignSaveSubsystem::GetSaveGameLoadingType( this ) != EExecGameLoading::SaveGame)
	{
		UUpliftCampaignSaveUtilities::AutoSave_Async( this, "StrategyStart", 0 );
	}
}

void AStrategyGameMode::InitializeForQuickPlay( )
{
	Super::InitializeForQuickPlay( );

	const auto CampaignState = ADS_Campaign::GetSingleton( this );

	const auto PIESettings = GetDefault< UCampaignPIESettings >( );

	for (const auto &HeroSpec : PIESettings->AdditionalRoster)
	{
		if (!HeroSpec.IsValid( ))
			continue;

		const auto Hero = ADS_Hero::SpawnHero( this, HeroSpec );
		CampaignState->AddToRoster( Hero );
	}
}

void AStrategyGameMode::PreTransitionOutOfMode( )
{
	Super::PreTransitionOutOfMode( );
}

void AStrategyGameMode::HandleNewFeatureActivations( const UObject *WorldContext, const TArray<const UStarfireFeatureData *> &NewFeatures )
{
	if (!ensureAlways( WorldContext ))
		return;
	
	const auto World = GEngine->GetWorldFromContextObject( WorldContext, EGetWorldErrorMode::LogAndReturnNull );
	if (!ensureAlways( World != nullptr))
		return;

	if (!ensureAlways(World->GetAuthGameMode(  )->IsA< AStrategyGameMode >( )))
		return; // mid-campaign features should only be activated during strategy

	// ---------------------------------------------------------------------------------------------------------------
	//   Allow the new features to add to the configuration of the campaign
	for (const auto Feature : NewFeatures)
	{
		if (!ensureAlways( Feature != nullptr ))
			continue;

		for (const auto Action : Feature->GetActions( ))
		{
			const auto CampaignAction = Cast< UGameFeatureAction_UpliftCampaign >( Action );
			if (CampaignAction == nullptr)
				continue;

			CampaignAction->OnCampaignInProgress( World );
		}
	}
}

#include "Kismet/BlueprintUtilitiesSF.h"

DEFINE_FUNCTION(AStrategyGameMode::execHandleNewFeatureActivations)
{
	P_GET_OBJECT(UObject,Z_Param_WorldContext);
	P_GET_TARRAY_REF(UStarfireFeatureData*,Z_Param_Out_NewFeatures);

	P_FINISH;

	P_NATIVE_BEGIN;

	AStrategyGameMode::HandleNewFeatureActivations( Z_Param_WorldContext, NativeCompatibilityCast( Z_Param_Out_NewFeatures ) );

	P_NATIVE_END;
}

#include "Misc/ExecSF.h"
#include "GameFeatures/FeatureContentManager.h"
#include "GameFeatures/UpliftGameFeatureData.h"
#include "Perf/SplatTaskManager.h"

using namespace ExecSF_Params;
struct FStrategyExecs : public FExecSF
{
	FStrategyExecs( )
	{
		AddExec( TEXT( "Uplift.Strategy.EnableMissingFeatures" ), TEXT( "Enable any legal mid-campaign features" ), FExecDelegate::CreateStatic( &FStrategyExecs::EnableFeatures ) );
	}

	static void EnableFeatures( const UWorld *World, const TCHAR *Cmd, FOutputDevice &Ar )
	{
		if (!World->GetAuthGameMode(  )->IsA< AStrategyGameMode >( ))
			return;

		const auto FeaturesManager = UFeatureContentManager::GetSubsystem( World );
		if (FeaturesManager == nullptr)
			return;

		auto Features = FeaturesManager->GetOwnedFeatures( );
		const TSet ActiveFeatures( FeaturesManager->GetEnabledFeatures( ) );

		Features.RemoveAll( [ &ActiveFeatures ]( const UStarfireFeatureData *Feature ) -> bool
		{
			if (ActiveFeatures.Contains( Feature ))
				return true; // Already active, don't reactivate

			if (Feature->ContentFlags.HasTag( UUpliftGameFeatureData::ContentFlag_NewGameOnly ))
				return true; // Not legal mid-campaign toggle

			if (Feature->ContentType.MatchesTag( UUpliftGameFeatureData::ContentType_Developer ))
				return true; // Developer tool that shouldn't be toggled
			
			return false;
		} );

		if (Features.IsEmpty( ))
			return;

		FeaturesManager->EnableFeatures( TSet( Features ) );
		
		auto AsyncProcess = [ WeakWorld = TWeakObjectPtr(World), Features, FeaturesManager ]( ) -> ESplatTaskResult
		{
			if (!WeakWorld.IsValid( ))
				return ESplatTaskResult::Complete;

			if (!FeaturesManager->AreEnabledFeaturesActive( ))
				return ESplatTaskResult::Yield;

			AStrategyGameMode::HandleNewFeatureActivations( WeakWorld.Get( ), Features );

			return ESplatTaskResult::Complete;
		};
		
		const auto TaskManager = USplatTaskManager::GetSubsystem( World );
		TaskManager->StartTask( MoveTemp( AsyncProcess ) );
	}
} GStrategyExecs;