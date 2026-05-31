// Copyright Russell Aasland. All Rights Reserved.

#pragma once

#include "DataStoreSingleton.h"
#include "Templates/ActorSingletonAccessor.h"

#include "GameplayRand.generated.h"

// Singleton source for controlling a specific random number stream that should be used for gameplay randomness.
// The standard FMath:: rand functions should be used for non-gameplay related fluff.
UCLASS( NotBlueprintable )
class CAMPAIGN_API ADS_GameplayRand : public ADataStoreSingleton, public TActorSingletonAccessors< ADS_GameplayRand >
{
	GENERATED_BODY( )
public:

	// Force the random number stream to a specific seed
	FORCEINLINE void Seed( int32 NewSeed )
	{
		Rand.Initialize( NewSeed );
	}

	// Returns a random float number in the range [0, 1).
	[[nodiscard]] FORCEINLINE float GetFraction( ) const
	{
		return Rand.GetFraction( );
	}

	// Returns a random number between 0 and MAX_UINT.
	[[nodiscard]] FORCEINLINE uint32 GetUnsignedInt() const
	{
		return Rand.GetUnsignedInt( );
	}

	// Returns a random vector of unit size.
	[[nodiscard]] FORCEINLINE FVector GetUnitVector( ) const
	{
		return Rand.GetUnitVector( );
	}

	// Returns a random float between 0 and 1, inclusive.
	[[nodiscard]] FORCEINLINE float FRand( ) const
	{
		return Rand.FRand( );
	}

	// Helper function for rand implementations. Returns a random number >= 0 and <= A
	[[nodiscard]] FORCEINLINE int32 RandHelper( int32 A ) const
	{
		return Rand.RandHelper( A );
	}

	// Helper function for rand implementations. Returns a random number >= Min and <= Max
	[[nodiscard]] FORCEINLINE int32 RandRange( int32 Min, int32 Max ) const
	{
		return Rand.RandRange( Min, Max );
	}

	// Helper function for rand implementations. Returns a random number >= Min and <= Max
	[[nodiscard]] FORCEINLINE float FRandRange( float Min, float Max ) const
	{
		return Rand.FRandRange( Min, Max );
	}

	// Returns a random vector of unit size.
	[[nodiscard]] FORCEINLINE FVector VRand( ) const
	{
		return Rand.VRand( );
	}

	// Returns a random point within the passed in bounding box
	[[nodiscard]] FORCEINLINE FVector RandPointInBox( const FBox &Box ) const
	{
		return Rand.RandPointInBox( Box );
	}

	// Returns a random unit vector, uniformly distributed, within the specified cone.
	[[nodiscard]] FORCEINLINE FVector VRandCone( const FVector &Dir, float ConeHalfAngleRad ) const
	{
		return Rand.VRandCone( Dir, ConeHalfAngleRad );
	}

	// Returns a random unit vector, uniformly distributed, within the specified cone.
	[[nodiscard]] FORCEINLINE FVector VRandCone( const FVector &Dir, float HorizontalConeHalfAngleRad, float VerticalConeHalfAngleRad ) const
	{
		return Rand.VRandCone( Dir, HorizontalConeHalfAngleRad, VerticalConeHalfAngleRad );
	}

	// Returns a random float number in the range [0, 1).
	UFUNCTION( BlueprintCallable, Category = "Gameplay Rand", meta = (WorldContext = "WorldContext") )
	[[nodiscard]] static float GetFraction( const UObject *WorldContext )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		return Rand->GetFraction( );
	}

	// Returns a random vector of unit size.
	UFUNCTION( BlueprintCallable, Category = "Gameplay Rands", meta = (WorldContext = "WorldContext") )
	[[nodiscard]] static FVector GetUnitVector( const UObject *WorldContext )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		return Rand->GetUnitVector( );
	}

	// Returns a random float between 0 and 1, inclusive.
	UFUNCTION( BlueprintCallable, Category = "Gameplay Rands", meta = (WorldContext = "WorldContext") )
	[[nodiscard]] static float FRand( const UObject *WorldContext )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		return Rand->FRand( );
	}

	// Helper function for rand implementations. Returns a random number >= 0 and <= A
	UFUNCTION( BlueprintCallable, Category = "Gameplay Rands", meta = (WorldContext = "WorldContext") )
	[[nodiscard]] static int32 RandHelper( const UObject *WorldContext, int32 A )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		return Rand->RandHelper( A );
	}

	// Helper function for rand implementations. Returns a random number >= Min and <= Max
	UFUNCTION( BlueprintCallable, Category = "Gameplay Rands", meta = (WorldContext = "WorldContext") )
	[[nodiscard]] static int32 RandRange( const UObject *WorldContext, int32 Min, int32 Max )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		return Rand->RandRange( Min, Max );
	}

	// Helper function for rand implementations. Returns a random number >= Min and <= Max
	UFUNCTION( BlueprintCallable, Category = "Gameplay Rands", meta = (WorldContext = "WorldContext") )
	[[nodiscard]] static float FRandRange( const UObject *WorldContext, float Min, float Max )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		return Rand->FRandRange( Min, Max );
	}

	// Returns a random vector of unit size.
	UFUNCTION( BlueprintCallable, Category = "Gameplay Rands", meta = (WorldContext = "WorldContext") )
	[[nodiscard]] static FVector VRand( const UObject *WorldContext )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		return Rand->VRand( );
	}

	// Returns a random point within the passed in bounding box
	UFUNCTION( BlueprintCallable, Category = "Gameplay Rands", meta = (WorldContext = "WorldContext", AutoCreateRefTerm = "Box") )
	[[nodiscard]] static FVector RandPointInBox(const UObject *WorldContext, const FBox& Box)
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		return Rand->RandPointInBox( Box );
	}

	// Returns a random unit vector, uniformly distributed, within the specified cone.
	UFUNCTION( BlueprintCallable, Category = "Gameplay Rands", meta = (WorldContext = "WorldContext", AutoCreateRefTerm = "Dir", DisplayName = "VRand Cone (Circle Cross Section)") )
	[[nodiscard]] static FVector VRandCone_Circle( const UObject *WorldContext, FVector const& Dir, float ConeHalfAngleRad )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		return Rand->VRandCone( Dir, ConeHalfAngleRad );
	}

	// Returns a random unit vector, uniformly distributed, within the specified cone.
	UFUNCTION( BlueprintCallable, Category = "Gameplay Rands", meta = (WorldContext = "WorldContext", AutoCreateRefTerm = "Dir", DisplayName = "VRand Cone (Ellipse Cross Section)") )
	[[nodiscard]] static FVector VRandCone_Ellipse( const UObject *WorldContext, FVector const& Dir, float HorizontalConeHalfAngleRad, float VerticalConeHalfAngleRad )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		return Rand->VRandCone( Dir, HorizontalConeHalfAngleRad, VerticalConeHalfAngleRad );
	}

	// Utility for shuffling the elements of an array
	template < class type_t >
	void Shuffle( TArray< type_t > &Array ) const
	{
		const int LastIndex = Array.Num( ) - 1;
		for (int idx = 0; idx < LastIndex; ++idx)
		{
			const int RandIdx = RandRange( idx, LastIndex );
			if (RandIdx != idx)
				Array.Swap( RandIdx, idx );
		}
	}

	// Utility for shuffling the elements of an array
	template < class type_t >
	static void Shuffle( const UObject *WorldContext, TArray< type_t > &Array )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		Rand->Shuffle( Array );
	}
	
	// Pick a random element from an array and return it and the index at which it exists
	template < class type_t >
	[[nodiscard]] const type_t& PickRandom( const TArray< type_t > &Array, int &OutIndex ) const
	{
		check( !Array.IsEmpty( ) );
		
		OutIndex = RandRange( 0, Array.Num( ) - 1 );
		return Array[ OutIndex ];
	}
	
	// Pick a random element from an array and return it and the index at which it exists
	template < class type_t >
	[[nodiscard]] type_t& PickRandom( TArray< type_t > &Array, int &OutIndex ) const
	{
		check( !Array.IsEmpty( ) );

		OutIndex = RandRange( 0, Array.Num( ) - 1 );
		return Array[ OutIndex ];
	}
	
	// Pick a random element from an array and return it
	template < class type_t >
	[[nodiscard]] const type_t& PickRandom( const TArray< type_t > &Array ) const
	{
		int IgnoredIndex;
		return PickRandom( Array, IgnoredIndex );
	}
	
	// Pick a random element from an array and return it
	template < class type_t >
	[[nodiscard]] type_t& PickRandom( TArray< type_t > &Array ) const
	{
		int IgnoredIndex;
		return PickRandom( Array, IgnoredIndex );
	}
	
	// Pick a random element from an array and return it
	template < class type_t >
	[[nodiscard]] static const type_t& PickRandom( const UObject *WorldContext, const TArray< type_t > &Array )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		int IgnoredIndex;
		return Rand->PickRandom( Array, IgnoredIndex );
	}
	
	// Pick a random element from an array and return it
	template < class type_t >
	[[nodiscard]] static type_t& PickRandom( const UObject *WorldContext, TArray< type_t > &Array )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		int IgnoredIndex;
		return Rand->PickRandom( Array, IgnoredIndex );
	}

	// Pick a random element, remove it and return it
	template < class type_t >
	[[nodiscard]] type_t RemoveRandom( TArray< type_t > &Array ) const
	{
		const auto RandIndex = RandRange( 0, Array.Num( ) - 1 );

		const auto Element = Array[ RandIndex ];
		Array.RemoveAt( RandIndex );
		return Element;
	}

	// Pick a random element, remove it and return it
	template < class type_t >
	[[nodiscard]] static type_t RemoveRandom( const UObject *WorldContext, TArray< type_t > &Array )
	{
		const auto Rand = GetSingleton( WorldContext );
		check( Rand != nullptr );

		return Rand->RemoveRandom( Array );
	}

	// Utility to create a number to that is good to use as a seed for GameplayRand
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Gameplay Rands" )
	[[nodiscard]] static int32 CreateNewSeed( )
	{
		return FMath::Rand( );
	}

private:
	// The random number stream
	UPROPERTY( VisibleInstanceOnly )
	FRandomStream Rand;

	// Utility for shuffling the elements of an array
	UFUNCTION( BlueprintCallable, CustomThunk, Category = "Gameplay Rands", meta = (DisplayName = "Shuffle", ArrayParm = "Array", WorldContext = "WorldContext") )
	static void Array_Shuffle( const UObject *WorldContext, const TArray< int32 > &Array );
	void GenericArray_Shuffle( const void *Array, const FArrayProperty *ArrayProp ) const;
	DECLARE_FUNCTION( execArray_Shuffle );
	// Implementation based on KismetArrayLibrary::Shuffle

	// Pick a random element from an array and return it (and the index at which it exists)
	UFUNCTION( BlueprintCallable, CustomThunk, Category = "Gameplay Rands", meta = (DisplayName = "Pick Random", ArrayParm = "Array", ArrayTypeDependentParams = "Item", AdvancedDisplay = "Index", WorldContext = "WorldContext"))
	static void Array_PickRandom( const UObject *WorldContext, const TArray< int32 > &Array, int32 &Item, int &Index );
	void GenericArray_PickRandom( const void *Array, const FArrayProperty *ArrayProp, void *Item, int &Index ) const;
	DECLARE_FUNCTION( execArray_PickRandom );
	// Implementation based on KismetArrayLibrary::Get

	// Pick a random element from an array, remove it and return it
	UFUNCTION( BlueprintCallable, CustomThunk, Category = "Gameplay Rands", meta = (DisplayName = "Remove Random", ArrayParm = "Array", ArrayTypeDependentParams = "Item", WorldContext = "WorldContext"))
	static void Array_RemoveRandom( const UObject *WorldContext, UPARAM( ref ) TArray< int32 > &Array, int32 &Item );
	void GenericArray_RemoveRandom( const void *Array, const FArrayProperty *ArrayProp, void *Item ) const;
	DECLARE_FUNCTION( execArray_RemoveRandom );
	// Implementation based on KismetArrayLibrary::Get
};