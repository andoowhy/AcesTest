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
class UComponentSparseArrayIterHandle;

UCLASS( BlueprintType, Blueprintable, Category = "Aces" )
class ACES_API UAcesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Aces" )
	TArray<TSubclassOf<UBaseSystem>> SystemClasses;

private:
	TArray<TComponentSparseArray> ComponentArrays;

	TArray<UScriptStruct*> IndexToComponentStruct;
	TMap<UScriptStruct*, uint32> ComponentStructToIndex;

	TArray<UBaseSystem*> Systems;
	FDelegateHandle TickDelegate;

	SIZE_T MaxComponent = 128;

	//~ Begin UGameInstanceSubsystem Interface.
	virtual void Initialize( FSubsystemCollectionBase& Collection ) override;
	virtual void Deinitialize() override;
	//~ End UGameInstanceSubsystem Interface.

public:
	UFUNCTION( BlueprintNativeEvent, Category = "Aces" )
	void AddAcesSystemClasses();
	virtual void AddAcesSystemClasses_Implementation();

	bool HandleTicker( float DeltaTime );

	//~ Begin Untyped/Blueprint API
public:
	TArray<UComponentSparseArrayHandle*> GetMatchingComponentArrays( const TArray<UScriptStruct*> ComponentScriptStructs );
	UComponentSparseArrayHandle* GetSmallestMatchingComponentArrayHandle( const TArray<UComponentSparseArrayHandle*> MatchingComponentArrays );

	//~ End Untyped/Blueprint API

	//~ Begin Template API
public:
	template<class ... ComponentTypes>
	void Each( TFunctionRef<void( ComponentTypes*... )> Func )
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
	FORCEINLINE void EachInvoke( TFunctionRef<void( ComponentTypes*... )>& Func,
								 TArray<TComponentSparseArray*, TFixedAllocator<sizeof...( ComponentTypes )>>& MatchingComponentArrays,
								 uint32 Entity,
								 TIntegerSequence<uint32, Indices...> )
	{
		Func( (ComponentTypes*)MatchingComponentArrays[Indices]->GetComponentData( Entity )... );
	};
	//~ End Template API
};
