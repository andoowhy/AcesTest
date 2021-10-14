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

FORCEINLINE bool UAcesBlueprintLibrary::IsEntityInAllComponentArrayHandles( UAcesSubsystem* const Aces, const FEntityHandle Entity, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles )
{
	check( Aces );
	return Aces->IsEntityInAllComponentArrayHandles( Entity, MatchingComponentArrayHandles );
}

FORCEINLINE FComponent& UAcesBlueprintLibrary::GetComponentData( const FEntityHandle Entity, const int32 ComponentArrayIndex, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles )
{
	check( MatchingComponentArrayHandles.IsValidIndex( ComponentArrayIndex ) );

	TComponentSparseArray* ComponentSparseArray = MatchingComponentArrayHandles[ComponentArrayIndex]->GetComponentSparseArray();
	check( ComponentSparseArray->IsValidEntity( Entity.Entity ) );
	return *static_cast<FComponent*>( ComponentSparseArray->GetComponentData( Entity.Entity ) );
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

FEntityHandle UAcesBlueprintLibrary::IterGetEntity( UComponentSparseArrayHandle* const ComponentSparseArrayHandle )
{
	check(ComponentSparseArrayHandle);
	return ComponentSparseArrayHandle->IterGetEntity();
}
