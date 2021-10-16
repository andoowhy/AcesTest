// Copyright 2021 Andrew Macdonald

#include "ComponentSparseArray.h"

#include "Containers/Array.h"
#include "Containers/ScriptArray.h"

TComponentSparseArray::TComponentSparseArray( uint32 MaxEntityCount, uint32 MaxComponentCount, UScriptStruct* ComponentStruct )
{
	this->MaxEntityCount = MaxComponentCount;
	this->MaxComponentCount = MaxComponentCount;
	this->ComponentStruct = ComponentStruct;

	this->SparseArrayTombstone = MaxEntityCount;
	this->DenseArrayTombstone = MaxComponentCount;

	DenseArrayCount = 0;

	SparseArray = TArray<uint32>();
	SparseArray.Reserve( MaxEntityCount );
	for( SIZE_T Index = 0; Index < MaxEntityCount; ++Index )
	{
		SparseArray.Add( SparseArrayTombstone );
	}

	DenseArrayCount = 0u;

	DenseArray = TArray<uint32>();
	DenseArray.Reserve( MaxComponentCount );
	for( SIZE_T Index = 0; Index < MaxComponentCount; ++Index )
	{
		DenseArray.Add( DenseArrayTombstone );
	}

	ComponentArray = new TScriptArray<FDefaultAllocator>();
	ComponentArray->Add( MaxComponentCount, ComponentStruct->GetStructureSize() );

	UScriptStruct::ICppStructOps* CPPStructOps = ComponentStruct->GetCppStructOps();
	for( SIZE_T Index = 0; Index < MaxComponentCount; ++Index )
	{
		CPPStructOps->Construct( GetComponentData( Index ) );
	}
}

TComponentSparseArray::~TComponentSparseArray()
{
	ComponentArray->Empty( 0, ComponentStruct->GetStructureSize() ); 
}

void* TComponentSparseArray::Create( uint32 Entity )
{
	if( IsValidEntity( Entity ) )
	{
		return GetComponentData( SparseArray[ Entity ] );
	}

	DenseArray[DenseArrayCount] = Entity;
	SparseArray[Entity] = DenseArrayCount;
	++DenseArrayCount;

	return GetComponentData( DenseArrayCount - 1 );
}

void TComponentSparseArray::Destroy( uint32 Entity )
{
	DenseArray[ SparseArray [ Entity ] ] = DenseArray[ DenseArrayCount - 1 ];
	SparseArray[ DenseArray[ DenseArrayCount - 1 ] ] = SparseArray[ Entity ];
	--DenseArrayCount;
}

TComponentSparseArray::TComponentSparseArrayIterator TComponentSparseArray::CreateIterator()
{
	return TComponentSparseArrayIterator( *this );
}

bool TComponentSparseArray::IsValidEntity( uint32 Entity ) const
{
	if( !SparseArray.IsValidIndex( Entity ) )
	{
		return false;
	}

	uint32 DenseIndex = SparseArray[ Entity ];
	return DenseIndex < DenseArrayTombstone && DenseArray[ DenseIndex ] == Entity;
}

FORCEINLINE void* TComponentSparseArray::GetComponentData( uint32 Index )
{
	return (void*)( (uint8*)ComponentArray->GetData() + Index * ComponentStruct->GetStructureSize() );
}

uint32 TComponentSparseArray::GetComponentNum()
{
	return DenseArray.Num();
}
