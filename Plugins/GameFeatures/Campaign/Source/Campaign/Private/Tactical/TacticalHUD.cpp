// Copyright Russell Aasland. All Rights Reserved.

#include "Tactical/TacticalHUD.h"

#include "Tactical/TacticalHUDWidget.h"
#include "Tactical/TacticalPawn.h"

// Engine
#include "Engine/Canvas.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TacticalHUD)

void ATacticalHUD::BeginPlay( )
{
	Super::BeginPlay( );

	if (HUDWidgetType != nullptr)
		HUDWidget = CastChecked< UTacticalHUDWidget >( CreateHUDWidget( HUDWidgetType ) );
}

void ATacticalHUD::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	HUDWidget = nullptr;
	
	Super::EndPlay( EndPlayReason );
}

// Names for each of the edge scrolling zones around the screen
static const FName UpperLeft( "ES_UL" );
static const FName UpperRight( "ES_UR" );
static const FName LowerLeft( "ES_LL" );
static const FName LowerRight( "ES_LR" );
static const FName Upper( "ES_U" );
static const FName Lower( "ES_Lo" );
static const FName LeftSide( "ES_Lt" );
static const FName RightSide( "ES_R" );

void ATacticalHUD::DrawHUD( )
{
	static constexpr int EdgeScrollThreshold = 32;
	static const FVector2D EdgeScrollRes( EdgeScrollThreshold, EdgeScrollThreshold );
	static const FVector2D EdgeScrollX( EdgeScrollThreshold, 0 );
	static const FVector2D EdgeScrollY( 0, EdgeScrollThreshold );

	Super::DrawHUD( );

	const FVector2D ScreenRes( Canvas->SizeX, Canvas->SizeY );
	const FVector2D ScreenResX( ScreenRes.X, 0 );
	const FVector2D ScreenResY( 0, ScreenRes.Y );

	const FVector2D OuterUL( 0, 0 );
	const FVector2D OuterUR( ScreenResX );
	const FVector2D OuterLL( ScreenResY );
	const FVector2D OuterLR( ScreenRes );

	const FVector2D InnerUL = OuterUL + EdgeScrollRes;
	const FVector2D InnerUR = OuterUR - EdgeScrollX + EdgeScrollY;
	const FVector2D InnerLL = OuterLL + EdgeScrollX - EdgeScrollY;
	const FVector2D InnerLR = OuterLR - EdgeScrollRes;

	AddHitBox( OuterUL, EdgeScrollRes, UpperLeft, true );
	AddHitBox( InnerUR - EdgeScrollY, EdgeScrollRes, UpperRight, true );
	AddHitBox( OuterLL - EdgeScrollY, EdgeScrollRes, LowerLeft, true );
	AddHitBox( InnerLR, EdgeScrollRes, LowerRight, true );

	AddHitBox( OuterUL + EdgeScrollX, ScreenResX - EdgeScrollX * 2 + EdgeScrollY, Upper, true );
	AddHitBox( OuterUL + EdgeScrollY, ScreenResY - EdgeScrollY * 2 + EdgeScrollX, LeftSide, true );
	AddHitBox( InnerUR, ScreenResY - EdgeScrollY * 2 + EdgeScrollX, RightSide, true );
	AddHitBox( InnerLL, ScreenResX - EdgeScrollX * 2 + EdgeScrollY, Lower, true );

	//RenderHitBoxes( DebugCanvas->Canvas );
}

void ATacticalHUD::NotifyHitBoxBeginCursorOver( FName BoxName )
{
	int Forward = 0, Right = 0;

	if ((BoxName == UpperLeft) || (BoxName == Upper) || (BoxName == UpperRight))
		Forward = 1;
	else if ((BoxName == LowerLeft) || (BoxName == Lower) || (BoxName == LowerRight))
		Forward = -1;

	if ((BoxName == UpperRight) || (BoxName == RightSide) || (BoxName == LowerRight))
		Right = 1;
	else if ((BoxName == UpperLeft) || (BoxName == LeftSide) || (BoxName == LowerLeft))
		Right = -1;

	if (const auto Pawn = Cast< ATacticalPawn >( GetOwningPawn( ) ))
		Pawn->UpdateEdgeScrollingSpeeds( Forward, Right );
}

void ATacticalHUD::NotifyHitBoxEndCursorOver( FName BoxName )
{
	if (const auto Pawn = Cast< ATacticalPawn >( GetOwningPawn() ))
		Pawn->UpdateEdgeScrollingSpeeds( 0, 0 );
}