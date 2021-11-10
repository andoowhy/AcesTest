// Copyright 2021 Andrew Macdonald

#include "AcesSubsystem.h"

#include "EntityHandle.h"
#include "ComponentSparseArrayHandle.h"

#include "Components/LocalTransform.h"
#include "Components/WorldTransform.h"

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
	
	for( SIZE_T Index = 0; Index < ComponentStructs.Num(); ++Index )
	{
		UScriptStruct* ComponentStruct = ComponentStructs[ Index ];
		uint16 index = IndexToComponentStruct.Num();

		IndexToComponentStruct.Add( ComponentStruct );
		ComponentStructToIndex.Add( ComponentStruct, index );
		TComponentSparseArray ComponentSparseArray = TComponentSparseArray( MaxEntityCount, UINT8_MAX, ComponentStruct );
		ComponentArrays.Add( MoveTemp( ComponentSparseArray ) );
	}

	TickDelegate = FTicker::GetCoreTicker().AddTicker( FTickerDelegate::CreateUObject( this, &UAcesSubsystem::HandleTicker ) );

	auto Entity = CreateEntity();
	auto* LocalTransform = CreateComponent<FLocalTransform>( Entity );
	auto* WorldTransform = CreateComponent<FWorldTransform>( Entity );
	LocalTransform->LocalTransform.SetTranslation( FVector( 1.0, 2.0, 3.0 ) );
	
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

	UpdateHelperActors();
	
	return true;
}

void UAcesSubsystem::UpdateHelperActors()
{
	// Update Unreal Helper Actors
	Each<FLocalTransform, FAcesStaticMesh>([&]( auto Entity, auto* LocalTransform, auto* AcesStaticMesh )
	{
		AcesStaticMesh->HelperActor->SetActorTransform(LocalTransform->LocalTransform);
	});
}

FEntityHandle UAcesSubsystem::CreateEntityHandle()
{
	FEntityHandle EntityHandle;
	EntityHandle.Entity = CreateEntity();
	return EntityHandle;
}

void UAcesSubsystem::DestroyEntityHandle( FEntityHandle Entity )
{
	DestroyEntity(Entity.Entity);
}

uint32 UAcesSubsystem::CreateEntity()
{
	if( DestroyedEntities.Num() > 0 )
	{	
		return DestroyedEntities.Pop();
	}
	else
	{
		uint32 Entity = FirstFreeEntity;
		FirstFreeEntity++;
		return Entity;
	}
}

void UAcesSubsystem::DestroyEntity( uint32 Entity )
{
	check( Entity < unsigned(MaxEntityCount));

	for( SIZE_T Index = 0; Index < ComponentArrays.Num(); ++Index )
	{
		ComponentArrays[Index].Destroy( Entity );
	}

	DestroyedEntities.Add(Entity);
}

TArray<UComponentSparseArrayHandle*> UAcesSubsystem::GetMatchingComponentArrayHandles( const TArray<UScriptStruct*> ComponentScriptStructs )
{
	TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles;
	MatchingComponentArrayHandles.Reserve( ComponentScriptStructs.Num() );

	for( UScriptStruct* ComponentScriptStruct : ComponentScriptStructs )
	{
		if (ComponentScriptStruct == nullptr)
		{
			continue;
		}

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
