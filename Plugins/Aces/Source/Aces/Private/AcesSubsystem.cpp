// Copyright 2021 Andrew Macdonald

#include "AcesSubsystem.h"

#include "EntityHandle.h"
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
	ComponentArrays.Reserve( ComponentStructs.Num() );
	
	for( SIZE_T Index = 0; Index < ComponentStructs.Num(); Index++ )
	{
		UScriptStruct* ComponentStruct = ComponentStructs[ Index ];
		uint16 index = IndexToComponentStruct.Num();

		IndexToComponentStruct.Add( ComponentStruct );
		ComponentStructToIndex.Add( ComponentStruct, index );
		ComponentArrays.Add( TComponentSparseArray( UINT16_MAX, UINT8_MAX, ComponentStruct ) );
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

TArray<UComponentSparseArrayHandle*> UAcesSubsystem::GetMatchingComponentArrayHandles( const TArray<UScriptStruct*> ComponentScriptStructs )
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

UComponentSparseArrayHandle* UAcesSubsystem::GetSmallestMatchingComponentArrayHandle( const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles )
{
	return *Algo::MinElementBy( MatchingComponentArrayHandles, [&]( const auto& MatchingComponentArrayHandle )
	{
		return MatchingComponentArrayHandle->GetComponentSparseArray()->GetComponentNum();
	} );
}

bool UAcesSubsystem::IsEntityInAllComponentArrayHandles(const FEntityHandle Entity, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles )
{
	return Algo::AllOf( MatchingComponentArrayHandles, [&]( const auto& ComponentArrayHandle )
	{
		return ComponentArrayHandle->GetComponentSparseArray()->IsValidEntity( Entity.Entity );
	} );
}
