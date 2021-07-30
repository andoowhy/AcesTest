// Copyright 2021 Andrew Macdonald

#include "AcesSubsystem.h"

#include "LocalTransform.h"

void UAcesSubsystem::Initialize( FSubsystemCollectionBase& Collection )
{
	// Register Component Classes and Initialize Component Arrays
	TArray<UScriptStruct*> ComponentStructs;
	for( TObjectIterator<UScriptStruct> StructIt; StructIt; ++StructIt )
	{
		UScriptStruct* Struct = ( *StructIt );
		if( Struct->IsChildOf( FComponent::StaticStruct() ) )
		{
			ComponentStructs.Add( Struct );
		}
	}

	IndexToComponentStruct.Reserve( ComponentStructs.Num() );
	ComponentStructToIndex.Reserve( ComponentStructs.Num() );

	for( UScriptStruct* ComponentStruct : ComponentStructs )
	{
		uint16 index = IndexToComponentStruct.Num();

		IndexToComponentStruct.Add( ComponentStruct );
		ComponentStructToIndex.Add( ComponentStruct, index );

		ComponentArrays.Emplace( TComponentSparseArray( UINT16_MAX, UINT8_MAX, ComponentStruct ) );
	}

	TickDelegate = FTicker::GetCoreTicker().AddTicker( FTickerDelegate::CreateUObject( this, &UAcesSubsystem::HandleTicker ) );

	AddAcesSystemClasses();

	// Create all systems
	for( TSubclassOf<UBaseSystem> SystemClass : SystemClasses )
	{
		Systems.Add( NewObject<UBaseSystem>( this, SystemClass ) );
	}
}

void UAcesSubsystem::Deinitialize()
{
	ComponentArrays.Empty();

	FTicker::GetCoreTicker().RemoveTicker( TickDelegate );
}

void UAcesSubsystem::AddAcesSystemClasses_Implementation()
{
}

bool UAcesSubsystem::HandleTicker( float DeltaTime )
{
	for( const auto& System : Systems )
	{
		System->Tick( this, DeltaTime );
	}

	return true;
}

TArray<UComponentSparseArrayHandle*> UAcesSubsystem::GetMatchingComponentArrays( const TArray<UScriptStruct*> ComponentScriptStructs )
{
	TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles;
	MatchingComponentArrayHandles.Reserve( ComponentScriptStructs.Num() );

	for( UScriptStruct* ComponentScriptStruct : ComponentScriptStructs )
	{
		MatchingComponentArrayHandles.Add(
			NewObject<UComponentSparseArrayHandle>()->Init(ComponentStructToIndex[ComponentScriptStruct])
		);
	}

	return MatchingComponentArrayHandles;
}

UComponentSparseArrayHandle* UAcesSubsystem::GetSmallestMatchingComponentArrayHandle( const TArray<uint32> MatchingComponentArrayIndices )
{
	uint32 SmallestMatchingComponentArrayIndex = *Algo::MinElementBy( MatchingComponentArrayIndices, [&]( const auto& MatchingComponentArrayIndex )
	{
		return ComponentArrays[MatchingComponentArrayIndex].GetComponentNum();
	} );

	return NewObject<UComponentSparseArrayHandle>()->Init(SmallestMatchingComponentArrayIndex);
}

bool UAcesSubsystem::IsEntityInAllComponentArrays( const uint32 Entity, const TArray<uint32> MatchingComponentArrayIndices )
{
	return Algo::AllOf( MatchingComponentArrayIndices, [&]( const auto& MatchingComponentArrayIndex )
	{
		return ComponentArrays[MatchingComponentArrayIndex].IsValidEntity( Entity );
	} );
}
