// Copyright 2021 Andrew Macdonald

#include "ComponentSparseArray.h"

#include "Containers/Array.h"
#include "Containers/ScriptArray.h"

TComponentSparseArray::TComponentSparseArray( uint32 MaxEntityCount, uint32 MaxComponentCount, UScriptStruct* ComponentStruct )
{
	this->MaxEntityCount = MaxComponentCount;
	this->MaxComponentCount = MaxComponentCount;
	this->ComponentStruct = ComponentStruct;

	DenseArrayCount = 0u;

	SparseArray = TArray<uint32>();
	SparseArray.Init( MaxEntityCount, MaxEntityCount );

	DenseArray = TArray<uint32>();
	DenseArray.Init( MaxComponentCount, MaxComponentCount );

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
}

void* TComponentSparseArray::Create( uint32 Entity )
{
	if( IsValidEntity( Entity ) )
	{
		return GetComponentData( SparseArray[ Entity ] );
	}

	DenseArray[DenseArrayCount] = Entity;
	SparseArray[Entity] = DenseArrayCount;
	DenseArrayCount++;

	return GetComponent( Entity );
}

void TComponentSparseArray::Destroy( uint32 Entity )
{
	if( !IsValidEntity( Entity ) )
	{
		return;
	}

	DenseArray[ SparseArray [ Entity ] ] = DenseArray[ DenseArrayCount - 1 ];
	SparseArray[ DenseArray[ DenseArrayCount - 1 ] ] = SparseArray[ Entity ];
	DenseArrayCount--;
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
	return DenseIndex < DenseArrayCount && DenseArray[ DenseIndex ] == Entity;
}

UScriptStruct* TComponentSparseArray::GetComponentStruct() const
{
	return ComponentStruct;
}

void* TComponentSparseArray::GetComponent( uint32 Entity )
{
	return GetComponentData( SparseArray[Entity] );
}

void* TComponentSparseArray::GetComponentData( uint32 Index )
{
	return (void*)( (uint8*)ComponentArray->GetData() + Index * ComponentStruct->GetStructureSize() );
}

uint32 TComponentSparseArray::GetComponentNum()
{
	return DenseArray.Num();
}
