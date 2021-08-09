// Copyright 2021 Andrew Macdonald

#include "AcesSubsystem.h"

#include "ComponentSparseArrayHandle.h"

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
			NewObject<UComponentSparseArrayHandle>()->Init(&ComponentArrays[ComponentStructToIndex[ComponentScriptStruct]])
		);
	}

	return MatchingComponentArrayHandles;
}

UComponentSparseArrayHandle* UAcesSubsystem::GetSmallestMatchingComponentArrayHandle( const TArray<UComponentSparseArrayHandle*> MatchingComponentArrays )
{
	return *Algo::MinElementBy( MatchingComponentArrays, [&]( const auto& MatchingComponentArray )
	{
		return MatchingComponentArray->GetComponentSparseArray()->GetComponentNum();
	} );
}
