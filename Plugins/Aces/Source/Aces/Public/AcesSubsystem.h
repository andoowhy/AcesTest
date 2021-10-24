// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ComponentSparseArray.h"
#include "Containers/Ticker.h"
#include "Algo/AllOf.h"
#include "Algo/MinElement.h"

#include "Component.h"
#include "AcesQuery.h"
#include "BaseSystem.h"

#include "AcesSubsystem.generated.h"

class UComponentSparseArrayHandle;
struct FEntityHandle;
class UComponentSparseArrayIterHandle;

UCLASS( BlueprintType, Blueprintable, Category = "Aces" )
class ACES_API UAcesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Aces", meta = ( ClampMin = "0") )
	int32 MaxEntityCount = UINT16_MAX;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Aces" )
	TArray<TSubclassOf<UBaseSystem>> SystemClasses;

private:
	uint32 FirstFreeEntity;
	TArray<uint32> DestroyedEntities;

	TArray<TComponentSparseArray> ComponentArrays;

	TArray<UScriptStruct*> IndexToComponentStruct;
	TMap<UScriptStruct*, uint32> ComponentStructToIndex;

	TArray<UBaseSystem*> Systems;
	FDelegateHandle TickDelegate;

	//~ Begin UGameInstanceSubsystem Interface.
	virtual void Initialize( FSubsystemCollectionBase& Collection ) override;
	virtual void Deinitialize() override;
	//~ End UGameInstanceSubsystem Interface.

public:
	UFUNCTION( BlueprintNativeEvent, Category = "Aces" )
	void AddAcesSystemClasses();
	virtual void AddAcesSystemClasses_Implementation();

	UFUNCTION( BlueprintCallable, Category = "Aces", meta = ( DisplayName = "Create Entity" ) )
	FEntityHandle CreateEntityHandle();

	UFUNCTION( BlueprintCallable, Category = "Aces", meta = ( DisplayName = "Destroy Entity" ) )
	void DestroyEntityHandle(FEntityHandle entity);

	uint32 CreateEntity();
	void DestroyEntity(uint32 Entity);

	bool HandleTicker( float DeltaTime );

	//~ Begin Untyped/Blueprint API
public:
	TArray<UComponentSparseArrayHandle*> GetMatchingComponentArrayHandles( const TArray<UScriptStruct*> ComponentScriptStructs );
	UComponentSparseArrayHandle* GetSmallestMatchingComponentArrayHandle( const TArray<UComponentSparseArrayHandle*> MatchingComponentArrays );
	bool IsEntityInAllComponentArrayHandles( const FEntityHandle Entity, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles  );

	//~ End Untyped/Blueprint API

	//~ Begin Template API
public:
	template<class ComponentType>
	ComponentType* CreateComponent( uint32 Entity )
	{
		static_assert( TIsDerivedFrom<ComponentType, FComponent>::Value, "Type is not an Aces Component" );

		TComponentSparseArray* ComponentSparseArray = &ComponentArrays[ComponentStructToIndex[ComponentType::StaticStruct()]];
		return (ComponentType*)ComponentSparseArray->Create( Entity );
	}

	template<class ... ComponentTypes>
	void Each( TFunctionRef<void( uint32, ComponentTypes*... )> Func )
	{
		static_assert( TAnd<TIsDerivedFrom<ComponentTypes, FComponent>... >::Value, "One or more types is not an Aces Component" );

		TArray<TComponentSparseArray*, TFixedAllocator<sizeof...( ComponentTypes )>> MatchingComponentArrays;
		for( auto ComponentScriptStruct : { ComponentTypes::StaticStruct()... } )
		{
			MatchingComponentArrays.Add( &ComponentArrays[ComponentStructToIndex[ComponentScriptStruct]] );
		}

		TComponentSparseArray* SmallestComponentArray = *Algo::MinElementBy( MatchingComponentArrays, []( const auto& ComponentArray )
		{
			return ComponentArray->GetComponentNum();
		} );

		for( auto It = SmallestComponentArray->CreateIterator(); It; ++It )
		{
			uint32 Entity = It.GetEntity();

			if( !IsEntityInAllComponentArrays<sizeof...( ComponentTypes )>(Entity, MatchingComponentArrays) )
			{
				continue;
			}  

			EachInvoke( Func, MatchingComponentArrays, Entity, TMakeIntegerSequence < uint32, sizeof...( ComponentTypes ) >{} );
		}
	};

private:
	template<uint32 ComponentTypeSize>
	FORCEINLINE bool IsEntityInAllComponentArrays(uint32 Entity, TArray<TComponentSparseArray*, TFixedAllocator<ComponentTypeSize>>& MatchingComponentArrays)
	{
		return Algo::AllOf( MatchingComponentArrays, [&]( const auto& ComponentArray )
		{
			return ComponentArray->IsValidEntity( Entity );
		} );
	} 

	template<class ... ComponentTypes, uint32 ... Indices>
	FORCEINLINE void EachInvoke( TFunctionRef<void( uint32, ComponentTypes*... )>& Func,
								 TArray<TComponentSparseArray*, TFixedAllocator<sizeof...( ComponentTypes )>>& MatchingComponentArrays,
								 uint32 Entity,
								 TIntegerSequence<uint32, Indices...> )
	{
		Func( Entity, (ComponentTypes*)MatchingComponentArrays[Indices]->GetComponent( Entity )... );
	};
	//~ End Template API
};
