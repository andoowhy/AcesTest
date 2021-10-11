// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"

class ACES_API TComponentSparseArray
{
	friend class UComponentSparseArrayHandle;

public:
	class TComponentSparseArrayIterator
	{
	public:
		TComponentSparseArrayIterator( TComponentSparseArray& InComponentSparseArray ) : ComponentSparseArray( InComponentSparseArray ) { }

		uint32 GetEntity() const
		{ 
			return ComponentSparseArray.DenseArray[Index];
		}

		TComponentSparseArrayIterator& operator++()
		{
			++Index;
			return *this;
		}

		explicit operator bool() const
		{
			return ComponentSparseArray.DenseArray.IsValidIndex( Index );
		}

	private:
		TComponentSparseArray& ComponentSparseArray;
		uint32 Index;
	};	
	
	TComponentSparseArray() = delete;
	TComponentSparseArray( uint32 MaxEntityCount, uint32 MaxComponentCount, UScriptStruct* ComponentStruct );

	~TComponentSparseArray();

	TComponentSparseArrayIterator CreateIterator();
	bool IsValidEntity( uint32 Entity ) const;
	void* GetComponentData( uint32 Index );
	uint32 GetComponentNum();

private:
	uint32 MaxEntityCount;
	uint32 MaxComponentCount;
	UScriptStruct* ComponentStruct;

	uint32 SparseArrayEnd;
	uint32 DenseArrayEnd;

	TArray<uint32> SparseArray;
	TArray<uint32> DenseArray;
	TScriptArray<FDefaultAllocator>* ComponentArray;
};
