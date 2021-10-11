// Copyright 2021 Andrew Macdonald

#include "ComponentSparseArrayHandle.h"

#include "EntityHandle.h"

UComponentSparseArrayHandle* UComponentSparseArrayHandle::Init( TComponentSparseArray* InComponentSparseArray )
{
	ComponentSparseArray = InComponentSparseArray;
	Iterator = new TComponentSparseArray::TComponentSparseArrayIterator( *InComponentSparseArray );

	return this;
}

FORCEINLINE FComponent& UComponentSparseArrayHandle::GetComponentData( const FEntityHandle Entity )
{
	check( ComponentSparseArray->IsValidEntity( Entity.Entity ) );
	return *static_cast<FComponent*>( ComponentSparseArray->GetComponentData( Entity.Entity ) );
}

void UComponentSparseArrayHandle::IterAdvance()
{
	check( Iterator );
	++( *Iterator );
}

bool UComponentSparseArrayHandle::IterIsValid()
{
	check( Iterator );
	return static_cast<bool>( *Iterator );
}

FEntityHandle UComponentSparseArrayHandle::IterGetEntity()
{
	check( Iterator );
	FEntityHandle EntityHandle;
	EntityHandle.Entity = Iterator->GetEntity();
	return EntityHandle;
}

void UComponentSparseArrayHandle::BeginDestroy()
{
	check( Iterator );
	delete Iterator;
	Super::BeginDestroy();
}
