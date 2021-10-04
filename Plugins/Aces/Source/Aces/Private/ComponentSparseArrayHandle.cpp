// Copyright 2021 Andrew Macdonald

#include "ComponentSparseArrayHandle.h"

UComponentSparseArrayHandle* UComponentSparseArrayHandle::Init( TComponentSparseArray* InComponentSparseArray )
{
	ComponentSparseArray = InComponentSparseArray;
	Iterator = new TComponentSparseArray::TComponentSparseArrayIterator( *InComponentSparseArray );

	return this;
}

FORCEINLINE FComponent& UComponentSparseArrayHandle::GetComponentData( const uint32 Entity )
{
	check( ComponentSparseArray->IsValidEntity( Entity ) );
	return *static_cast<FComponent*>( ComponentSparseArray->GetComponentData( Entity ) );
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

uint32 UComponentSparseArrayHandle::IterGetEntity()
{
	check( Iterator );
	return Iterator->GetEntity();
}

void UComponentSparseArrayHandle::BeginDestroy()
{
	check( Iterator );
	delete Iterator;
	Super::BeginDestroy();
}
