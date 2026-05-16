// Copyright Russell Aasland. All Rights Reserved.

#include "Shell/ShellHUDWidget.h"

#include "Shell/ShellExtensionWidget.h"
#include "Shell/GameFeatureAction_AddShellWidgets.h"

#include "GameFeatures/StarfireFeatureData.h"
#include "GameFeatures/FeatureContentManager.h"

// UMG
#include "Components/VerticalBox.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ShellHUDWidget)

void UShellHUDWidget::NativeOnActivated( )
{
	Super::NativeOnActivated( );

	const auto FeaturesManager = UFeatureContentManager::GetSubsystem( this );
	check( FeaturesManager != nullptr );

	FeaturesManager->OnFeatureOwned.AddDynamic( this, &UShellHUDWidget::OnOwnedFeatureSetChanged );
	FeaturesManager->OnFeatureDisowned.AddDynamic( this, &UShellHUDWidget::OnOwnedFeatureSetChanged );

	if (IsValid( ExtensionButtons ))
		AddExtensionWidgets( ExtensionButtons );
}

void UShellHUDWidget::NativeOnDeactivated( )
{
	const auto FeaturesManager = UFeatureContentManager::GetSubsystem( this );
	check( FeaturesManager != nullptr );

	FeaturesManager->OnFeatureOwned.RemoveAll( this );
	FeaturesManager->OnFeatureDisowned.RemoveAll( this );

	Super::NativeOnDeactivated( );
}

void UShellHUDWidget::AddExtensionWidgets( UVerticalBox *ButtonsContainer )
{
	// For now, just get rid of everything and rebuild it from scratch
	ButtonsContainer->ClearChildren( );

	const auto FeaturesManager = UFeatureContentManager::GetSubsystem( this );
	check( FeaturesManager != nullptr );

	const auto Features = FeaturesManager->GetOwnedFeatures( );

	// Find all the features that add main menu buttons
	TArray< const UGameFeatureAction_AddShellWidgets* > ButtonGroups;
	for (const auto &F : Features)
	{
		for (const auto &A : F->GetActions( ))
		{
			if (const auto B = Cast< UGameFeatureAction_AddShellWidgets >( A ))
				ButtonGroups.Push( B );
		}
	}

	auto SortPredicate = [ ]( const UGameFeatureAction_AddShellWidgets &A, const UGameFeatureAction_AddShellWidgets &B ) -> bool
	{
		return A.Priority > B.Priority;
	};
	ButtonGroups.Sort( SortPredicate );

	// Create and add buttons from all the game features
	for (const auto &A : ButtonGroups)
	{
		for (const auto &B : A->AdditionalButtons)
		{
			const auto ButtonClass = B.Get( );
			if (ensureAlways( ButtonClass != nullptr ))
			{
				const auto NewButton = CreateWidget< UShellExtensionWidget >( this, ButtonClass );
				ButtonsContainer->AddChild( NewButton );
			}
		}
	}
}

void UShellHUDWidget::OnOwnedFeatureSetChanged( const UStarfireFeatureData *FeatureData )
{
	if (IsValid( ExtensionButtons ))
		AddExtensionWidgets( ExtensionButtons );
}
