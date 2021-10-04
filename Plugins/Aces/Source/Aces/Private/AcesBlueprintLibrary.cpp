// Copyright 2021 Andrew Macdonald

#include "AcesBlueprintLibrary.h"

FORCEINLINE TArray<UComponentSparseArrayHandle*> UAcesBlueprintLibrary::GetMatchingComponentArrayHandles( UAcesSubsystem* const Aces, const TArray<UScriptStruct*> ComponentScriptStructs )
{
	check( Aces );
	return Aces->GetMatchingComponentArrayHandles( ComponentScriptStructs );
}

FORCEINLINE UComponentSparseArrayHandle* UAcesBlueprintLibrary::GetSmallestMatchingComponentArrayHandle( UAcesSubsystem* const Aces, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles )
{
	check( Aces );
	return Aces->GetSmallestMatchingComponentArrayHandle( MatchingComponentArrayHandles );
}

FORCEINLINE bool UAcesBlueprintLibrary::IsEntityInAllComponentArrayHandles( UAcesSubsystem* const Aces, const uint32 Entity, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles )
{
	check( Aces );
	return Aces->IsEntityInAllComponentArrayHandles( Entity, MatchingComponentArrayHandles );
}

FORCEINLINE FComponent& UAcesBlueprintLibrary::GetComponentData( const uint32 Entity, UComponentSparseArrayHandle* const ComponentSparseArrayHandle )
{
	check( ComponentSparseArrayHandle );
	TComponentSparseArray* ComponentSparseArray = ComponentSparseArrayHandle->GetComponentSparseArray();
	check( ComponentSparseArray->IsValidEntity( Entity ) );
	return *static_cast<FComponent*>( ComponentSparseArray->GetComponentData( Entity ) );
}

FORCEINLINE void UAcesBlueprintLibrary::IterAdvance( UComponentSparseArrayHandle* const ComponentSparseArrayHandle )
{
	check(ComponentSparseArrayHandle);
	ComponentSparseArrayHandle->IterAdvance();
}

bool UAcesBlueprintLibrary::IterIsValid( UComponentSparseArrayHandle* const ComponentSparseArrayHandle )
{
	check(ComponentSparseArrayHandle);
	return ComponentSparseArrayHandle->IterIsValid();
}

uint32 UAcesBlueprintLibrary::IterGetEntity( UComponentSparseArrayHandle* const ComponentSparseArrayHandle )
{
	check(ComponentSparseArrayHandle);
	return ComponentSparseArrayHandle->IterGetEntity();
}
