// Copyright 2021 Andrew Macdonald

#include "AcesBlueprintLibrary.h"

FORCEINLINE TArray<UComponentSparseArrayHandle*> UAcesBlueprintLibrary::GetMatchingComponentArrays( UAcesSubsystem* const Aces, const TArray<UScriptStruct*> ComponentScriptStructs )
{
	return Aces->GetMatchingComponentArrays( ComponentScriptStructs );
}

FORCEINLINE UComponentSparseArrayHandle* UAcesBlueprintLibrary::GetSmallestMatchingComponentArrayHandle( UAcesSubsystem* const Aces, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrays )
{
	return Aces->GetSmallestMatchingComponentArrayHandle( MatchingComponentArrays );
}

FORCEINLINE bool UAcesBlueprintLibrary::IsValidEntity( const uint32 Entity, UComponentSparseArrayHandle* const ComponentSparseArrayHandle )
{
	return ComponentSparseArrayHandle->GetComponentSparseArray()->IsValidEntity(Entity);
}

FORCEINLINE uint32 UAcesBlueprintLibrary::GetComponentNum( UComponentSparseArrayHandle* const ComponentSparseArrayHandle )
{
	return ComponentSparseArrayHandle->GetComponentSparseArray()->GetComponentNum();
}

FORCEINLINE FComponent& UAcesBlueprintLibrary::GetComponentData( const uint32 Entity, UComponentSparseArrayHandle* const ComponentSparseArrayHandle )
{
	TComponentSparseArray* ComponentSparseArray = ComponentSparseArrayHandle->GetComponentSparseArray();
	check( ComponentSparseArray->IsValidEntity( Entity ) );
	return *static_cast<FComponent*>( ComponentSparseArray->GetComponentData( Entity ) );
}
