// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "GameWorld/CampaignPawn.h"

#include "TacticalPawn.generated.h"

struct FInputActionValue;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

// Base type for game states to be associated with TacticalGameMode
UCLASS( )
class CAMPAIGN_API ATacticalPawn : public ACampaignPawn
{
	GENERATED_BODY( )
public:
	ATacticalPawn( );

	// Actor API
	void BeginPlay( ) override;
	void Tick( float DeltaSeconds ) override;

	// Pawn API
	void SetupPlayerInputComponent( UInputComponent* PlayerInputComponent ) override;
	void UnPossessed( ) override;

	// Update the speed at which the pawn should be panning in the x/y plane
	void UpdateEdgeScrollingSpeeds( int Forward, int Right );

protected:
	// Utility to reposition the camera based on the current camera values
	void UpdateCamera( void );

	// Handle panning of the camera in the x/y plane
	void HandlePanning( const FVector2D &Value );

	// The component for the camera
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Tactical Camera" )
	TObjectPtr< UCameraComponent > Camera;

	// Distance of the camera from the pawn location
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Tactical Camera" )
	float CameraRadius;

	// Rotation of Camera around pawn local z-axis
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Tactical Camera" )
	float CameraRotation;

	// Angle of Camera from pawn load z-axis
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Tactical Camera" )
	float CameraHeight;
	
	// Speed to move the pawn when panning
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Tactical Camera" )
	float MovementSpeed;

	// Speed to rotate about the local origin
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Tactical Camera" )
	float RotationSpeed;

	// Speed to move towards or away from the local origin
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Tactical Camera" )
	float ZoomSpeed;

	// Smallest allowable distance from the local origin
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Tactical Camera" )
	float RadiusMin;

	// Largest allowable distance from the local origin
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Tactical Camera" )
	float RadiusMax;

	// Speed to pan the pawn along local-x
	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly, Category = "Tactical Camera" )
	float EdgeScrollingForwardSpeed;

	// Speed to pan the pawn long local-y
	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly, Category = "Tactical Camera" )
	float EdgeScrollingRightSpeed;

	// Input mapping for camera controls
	UPROPERTY( EditDefaultsOnly, Category = "Tactical Camera|Input", meta = (DisplayThumbnail = false) )
	TObjectPtr< const UInputMappingContext > CameraInputContext = nullptr;

	// Input Action for camera panning movement
	UPROPERTY( EditDefaultsOnly, Category = "Tactical Camera|Input", meta = (DisplayThumbnail = false) )
	TObjectPtr< const UInputAction > PanAction;

	// Input Action for camera zoom in & out
	UPROPERTY( EditDefaultsOnly, Category = "Tactical Camera|Input", meta = (DisplayThumbnail = false) )
	TObjectPtr< const UInputAction > ZoomAction;

	// Input Action for camera rotation around target
	UPROPERTY( EditDefaultsOnly, Category = "Tactical Camera|Input", meta = (DisplayThumbnail = false) )
	TObjectPtr< const UInputAction > RotateAction;

	// Move the camera "forward" (as defined by the X-axis of the current camera orientation)
	// or "right" (as defined by the Y-axis of the current camera orientation)
	void HandlePanning( const FInputActionValue &Value );
	// Rotate the camera around Z
	void HandleRotation( const FInputActionValue &Value );
	// Update the camera position to be moved towards or away from the base actor
	void HandleZoom( const FInputActionValue &Value );
};