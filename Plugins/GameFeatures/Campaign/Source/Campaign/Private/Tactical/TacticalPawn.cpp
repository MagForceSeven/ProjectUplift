// Copyright Russell Aasland. All Rights Reserved.

#include "Tactical/TacticalPawn.h"

// Enhanced Input
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

// Engine
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TacticalPawn)

ATacticalPawn::ATacticalPawn( )
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	USceneComponent *Root = CreateDefaultSubobject< USceneComponent >( "Root" );

	Camera = CreateDefaultSubobject< UCameraComponent >( "Camera" );
	Camera->SetupAttachment( Root );
	Camera->bUsePawnControlRotation = false;

	CameraRadius = 1024.0f;
	CameraRotation = 0.0f;
	CameraHeight = FMath::DegreesToRadians( 45.0f );
	
	MovementSpeed = 5.0f;
	RotationSpeed = FMath::DegreesToRadians( 2.0f );
	ZoomSpeed = 16.0f;

	RadiusMin = 128.0f;
	RadiusMax = 2048.0f;

	EdgeScrollingForwardSpeed = 0.0f;
	EdgeScrollingRightSpeed = 0.0f;

	OverrideInputComponentClass = UEnhancedInputComponent::StaticClass( );
}

void ATacticalPawn::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
	Super::SetupPlayerInputComponent( PlayerInputComponent );

	if (CameraInputContext != nullptr)
	{
		ensureAlways( CameraInputContext->GetRegistrationTrackingMode( ) == EMappingContextRegistrationTrackingMode::CountRegistrations );

		if (const auto InputSubsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem >( CastChecked< APlayerController >( Controller )->GetLocalPlayer( ) ) )
			InputSubsystem->AddMappingContext( CameraInputContext, 0 );
	}

	const auto EnhancedInput = CastChecked< UEnhancedInputComponent >( PlayerInputComponent );

	if (PanAction != nullptr)
	{
		ensureAlways( PanAction->ValueType == EInputActionValueType::Axis2D );
		EnhancedInput->BindAction( PanAction, ETriggerEvent::Triggered, this, &ATacticalPawn::HandlePanning );
	}
	if (ZoomAction != nullptr)
	{
		ensureAlways( ZoomAction->ValueType == EInputActionValueType::Axis1D );
		EnhancedInput->BindAction( ZoomAction, ETriggerEvent::Triggered, this, &ATacticalPawn::HandleZoom );
	}
	if (RotateAction != nullptr)
	{
		ensureAlways( RotateAction->ValueType == EInputActionValueType::Axis1D );
		EnhancedInput->BindAction( RotateAction, ETriggerEvent::Triggered, this, &ATacticalPawn::HandleRotation );
	}
}

void ATacticalPawn::UnPossessed( )
{
	if (CameraInputContext != nullptr)
	{
		if (const auto InputSubsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem >( CastChecked< APlayerController >( Controller )->GetLocalPlayer( ) ) )
			InputSubsystem->RemoveMappingContext( CameraInputContext );
	}

	Super::UnPossessed( );
}

void ATacticalPawn::BeginPlay( )
{
	Super::BeginPlay( );

	UpdateCamera( );
}

void ATacticalPawn::Tick( float DeltaSeconds )
{
	Super::Tick( DeltaSeconds );

	if ((EdgeScrollingForwardSpeed != 0.0f) || (EdgeScrollingRightSpeed != 0.0f))
		HandlePanning( FVector2D( EdgeScrollingRightSpeed, EdgeScrollingForwardSpeed ) );
}

void ATacticalPawn::UpdateEdgeScrollingSpeeds( int Forward, int Right )
{
	EdgeScrollingForwardSpeed = Forward;
	EdgeScrollingRightSpeed = Right;
}

static FVector PointOnSphere( float Radius, float AngleZ, float AngleH )
{
	return FVector( Radius * FMath::Sin( AngleH ) * FMath::Cos( AngleZ ),
					Radius * FMath::Sin( AngleH ) * FMath::Sin( AngleZ ),
					Radius * FMath::Cos( AngleH ) );
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATacticalPawn::UpdateCamera( void )
{
	const FVector NewLocation = PointOnSphere( CameraRadius, CameraRotation, CameraHeight );
	const FRotator NewRotation = FRotationMatrix::MakeFromX( FVector::ZeroVector - NewLocation ).Rotator( );

	Camera->SetRelativeLocationAndRotation( NewLocation, NewRotation );
}

void ATacticalPawn::HandlePanning( const FVector2D &Value )
{
	auto CameraRotator = Camera->GetComponentRotation( );
	CameraRotator.Pitch = 0.0f;
	CameraRotator.Roll = 0.0f;
	const auto Forward = CameraRotator.Vector( );
	const auto Right = FRotationMatrix( CameraRotator ).GetScaledAxis( EAxis::Y );

	const auto Forward_Scalar = Value.Y * MovementSpeed;
	const auto Right_Scalar = Value.X * MovementSpeed;

	SetActorLocation( GetActorLocation( ) + (Right * Right_Scalar) + (Forward * Forward_Scalar) );
}

void ATacticalPawn::HandlePanning( const FInputActionValue &Value )
{
	const auto Movement = Value.Get< FVector2D >( );
	HandlePanning( Movement );
}

void ATacticalPawn::HandleRotation( const FInputActionValue &Value )
{
	const auto Movement = Value.Get< float >( );

	CameraRotation += RotationSpeed * Movement;

	UpdateCamera( );
}

void ATacticalPawn::HandleZoom( const FInputActionValue &Value )
{
	const auto Movement = Value.Get< float >( );
	
	CameraRadius -= ZoomSpeed * Movement;
	CameraRadius = FMath::Clamp( CameraRadius, RadiusMin, RadiusMax );

	UpdateCamera( );
}
