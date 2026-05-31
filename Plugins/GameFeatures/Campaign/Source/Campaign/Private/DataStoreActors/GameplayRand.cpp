// Copyright Russell Aasland. All Rights Reserved.

#include "DataStoreActors/GameplayRand.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayRand)

void ADS_GameplayRand::Array_Shuffle( const UObject*, const TArray< int32 >& )
{
	// Should never hit this! Stubbed out since the class is not NoExport. Calls should all be routing to GenericArray_Shuffle
	check( 0 );
}

DEFINE_FUNCTION( ADS_GameplayRand::execArray_Shuffle )
{
	P_GET_OBJECT( UObject, Z_Param_WorldContext );

	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn< FArrayProperty >( nullptr );
	void *ArrayAddr = Stack.MostRecentPropertyAddress;
	const auto ArrayProperty = CastField< FArrayProperty >( Stack.MostRecentProperty );
	if (ArrayProperty == nullptr)
	{
		Stack.bArrayContextFailed = true;
		return;
	}

	P_FINISH;

	P_NATIVE_BEGIN;
	const auto GameplayRand = GetSingleton( Z_Param_WorldContext );
	GameplayRand->GenericArray_Shuffle( ArrayAddr, ArrayProperty );
	P_NATIVE_END;
}

void ADS_GameplayRand::GenericArray_Shuffle( const void *Array, const FArrayProperty *ArrayProp ) const
{
	if (Array == nullptr)
		return;

	FScriptArrayHelper ArrayHelper( ArrayProp, Array );
	const auto LastIndex = ArrayHelper.Num( ) - 1;
	for (int idx = 0; idx < LastIndex; ++idx)
	{
		const auto RandIdx = RandRange( idx, LastIndex );
		if (idx != RandIdx)
			ArrayHelper.SwapValues( idx, RandIdx );
	}
}

void ADS_GameplayRand::Array_PickRandom( const UObject *WorldContext, const TArray< int32 > &Array, int32 &Item, int &Index )
{
	// Should never hit this! Stubbed out since the class is not NoExport. Calls should all be routing to GenericArray_PickRandom
	check( 0 );
}

void ADS_GameplayRand::GenericArray_PickRandom( const void *Array, const FArrayProperty *ArrayProp, void *Item, int &Index ) const
{
	if (Array == nullptr)
		return;

	FScriptArrayHelper ArrayHelper( ArrayProp, Array );
	if (ArrayHelper.Num( ) == 0) // array helper apparently doesn't have a IsEmpty method
		return;

	Index = FRandRange( 0, ArrayHelper.Num( ) - 1 );

	const auto InnerProp = ArrayProp->Inner;
	InnerProp->CopyCompleteValueFromScriptVM( Item, ArrayHelper.GetRawPtr( Index ) );
}

DEFINE_FUNCTION( ADS_GameplayRand::execArray_PickRandom )
{
	P_GET_OBJECT( UObject,Z_Param_WorldContext );

	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn< FArrayProperty >( nullptr );
	const void *ArrayAddr = Stack.MostRecentPropertyAddress;
	const auto ArrayProperty = CastField< FArrayProperty >( Stack.MostRecentProperty );
	if (ArrayProperty == nullptr)
	{
		Stack.bArrayContextFailed = true;
		return;
	}

	// Since Item isn't really an int, step the stack manually
	const auto InnerProp = ArrayProperty->Inner;
	const auto PropertySize = InnerProp->GetElementSize( ) * InnerProp->ArrayDim;
	const auto StorageSpace = FMemory_Alloca( PropertySize );
	InnerProp->InitializeValue( StorageSpace );

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn< FProperty >( StorageSpace );
	const auto InnerPropClass = InnerProp->GetClass( );
	const auto MostRecentPropClass = Stack.MostRecentProperty->GetClass( );
	void *ItemPtr = StorageSpace;
	// If the destination and the inner type are identical in size and their field classes derive from one another, then permit the writing out of the array element to the destination memory
	if ((Stack.MostRecentPropertyAddress != nullptr) && (PropertySize == Stack.MostRecentProperty->GetElementSize( ) * Stack.MostRecentProperty->ArrayDim) &&
		(MostRecentPropClass->IsChildOf( InnerPropClass ) || InnerPropClass->IsChildOf( MostRecentPropClass )))
	{
		ItemPtr = Stack.MostRecentPropertyAddress;
	}

	P_GET_PROPERTY_REF( FIntProperty, OutIndex );

	P_FINISH;

	P_NATIVE_BEGIN;
	const auto GameplayRand = GetSingleton( Z_Param_WorldContext );
	GameplayRand->GenericArray_PickRandom( ArrayAddr, ArrayProperty, ItemPtr, OutIndex );
	P_NATIVE_END;
}


void ADS_GameplayRand::Array_RemoveRandom( const UObject *WorldContext, TArray< int32 > &Array, int32 &Item )
{
	// Should never hit this! Stubbed out since the class is not NoExport. Calls should all be routing to GenericArray_RemoveRandom
	check( 0 );
}

void ADS_GameplayRand::GenericArray_RemoveRandom( const void *Array, const FArrayProperty *ArrayProp, void *Item ) const
{
	if (Array == nullptr)
		return;

	FScriptArrayHelper ArrayHelper( ArrayProp, Array );
	if (ArrayHelper.Num( ) == 0) // array helper apparently doesn't have a IsEmpty method
		return;

	const auto Index = FRandRange( 0, ArrayHelper.Num( ) - 1 );

	const auto InnerProp = ArrayProp->Inner;
	InnerProp->CopyCompleteValueFromScriptVM( Item, ArrayHelper.GetRawPtr( Index ) );
	ArrayHelper.RemoveValues( Index, 1 );
}

DEFINE_FUNCTION( ADS_GameplayRand::execArray_RemoveRandom )
{
	P_GET_OBJECT( UObject,Z_Param_WorldContext );

	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn< FArrayProperty >( nullptr );
	const void *ArrayAddr = Stack.MostRecentPropertyAddress;
	const auto ArrayProperty = CastField< FArrayProperty >( Stack.MostRecentProperty );
	if (ArrayProperty == nullptr)
	{
		Stack.bArrayContextFailed = true;
		return;
	}

	// Since Item isn't really an int, step the stack manually
	const auto InnerProp = ArrayProperty->Inner;
	const auto PropertySize = InnerProp->GetElementSize( ) * InnerProp->ArrayDim;
	const auto StorageSpace = FMemory_Alloca( PropertySize );
	InnerProp->InitializeValue( StorageSpace );

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn< FProperty >( StorageSpace );
	const auto InnerPropClass = InnerProp->GetClass( );
	const auto MostRecentPropClass = Stack.MostRecentProperty->GetClass( );
	void *ItemPtr = StorageSpace;
	// If the destination and the inner type are identical in size and their field classes derive from one another, then permit the writing out of the array element to the destination memory
	if ((Stack.MostRecentPropertyAddress != nullptr) && (PropertySize == Stack.MostRecentProperty->GetElementSize( ) * Stack.MostRecentProperty->ArrayDim) &&
		(MostRecentPropClass->IsChildOf( InnerPropClass ) || InnerPropClass->IsChildOf( MostRecentPropClass )))
	{
		ItemPtr = Stack.MostRecentPropertyAddress;
	}

	P_FINISH;

	P_NATIVE_BEGIN;
	const auto GameplayRand = GetSingleton( Z_Param_WorldContext );
	GameplayRand->GenericArray_RemoveRandom( ArrayAddr, ArrayProperty, ItemPtr );
	P_NATIVE_END;
}