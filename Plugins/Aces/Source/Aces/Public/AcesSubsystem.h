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
	TMap<UScriptStruct*, uint16> ComponentStructToIndex;

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

	template<class ... ComponentTypes, uint32 ... Indices>
	FORCEINLINE void Invoke( TFunctionRef<void( ComponentTypes*... )>& Func,
							 TArray<TComponentSparseArray*, TFixedAllocator<sizeof...( ComponentTypes )>>& MatchingComponentArrays,
							 uint32 Entity,
							 TIntegerSequence<uint32, Indices...> )
	{
		Func( (ComponentTypes*)MatchingComponentArrays[Indices]->GetComponentData( Entity )... );
	};

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

			if( !Algo::AllOf( MatchingComponentArrays, [&]( const auto& ComponentArray )
			{
				return ComponentArray->IsValidEntity( Entity );
			} ) )
			{
				continue;
			}

			Invoke( Func, MatchingComponentArrays, Entity, TMakeIntegerSequence < uint32, sizeof...( ComponentTypes ) >{} );
		}
	};
};
