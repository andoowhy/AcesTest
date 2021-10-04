// Copyright 2021 Andrew Macdonald

#include "ComponentSparseArray.h"

#include "Containers/Array.h"
#include "Containers/ScriptArray.h"

TComponentSparseArray::TComponentSparseArray( uint32 MaxEntityCount, uint32 MaxComponentCount, UScriptStruct* ComponentStruct )
{
	this->MaxEntityCount = MaxComponentCount;
	this->MaxComponentCount = MaxComponentCount;
	this->ComponentStruct = ComponentStruct;

	this->SparseArrayEnd = MaxEntityCount - 1;
	this->DenseArrayEnd = MaxComponentCount - 1;

	SparseArray = TArray<uint32>();
	SparseArray.Reserve( MaxEntityCount );
	for (SIZE_T Index = 0; Index < MaxEntityCount; ++Index)
	{
		SparseArray.Add(SparseArrayEnd);
	}

	DenseArray = TArray<uint32>();
	DenseArray.Reserve( MaxComponentCount );
	for( SIZE_T Index = 0; Index < MaxComponentCount; ++Index )
	{
		DenseArray.Add(DenseArrayEnd);
	}

	ComponentArray.Add( MaxComponentCount, ComponentStruct->GetStructureSize() );
	UScriptStruct::ICppStructOps* CPPStructOps = ComponentStruct->GetCppStructOps();
	for( SIZE_T Index = 0; Index < MaxComponentCount; ++Index )
	{
		CPPStructOps->Construct( GetComponentData( Index ) );
	}
}

TComponentSparseArray::TComponentSparseArrayIterator TComponentSparseArray::CreateIterator()
{
	return TComponentSparseArrayIterator( *this );
}

bool TComponentSparseArray::IsValidEntity( uint32 Entity ) const
{
	return SparseArray.IsValidIndex( Entity ) && SparseArray[Entity] < SparseArrayEnd;
}

FORCEINLINE void* TComponentSparseArray::GetComponentData( uint32 Index )
{
	return (void*)( (uint8*)ComponentArray.GetData() + Index * ComponentStruct->GetStructureSize() );
}

uint32 TComponentSparseArray::GetComponentNum()
{
	return DenseArray.Num();
}
