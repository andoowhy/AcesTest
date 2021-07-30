// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "ComponentSparseArray.h"

#include "ComponentSparseArrayIterHandle.generated.h"

UCLASS(BlueprintInternalUseOnly)
class ACES_API UComponentSparseArrayIterHandle : public UObject
{
	GENERATED_BODY()
public:
	UComponentSparseArrayIterHandle* Init(TComponentSparseArray::TComponentSparseArrayIterator& InIter)
	{
		Iter = &InIter;
		return this;
	}

	UFUNCTION()
	uint32 GetEntity(){ return Iter->GetEntity(); }

	UFUNCTION()
	void Advance(){ Iter++; }

	UFUNCTION()
	bool IsValid(){ return Iter->operator bool(); }

private:
	TComponentSparseArray::TComponentSparseArrayIterator* Iter = nullptr;
};
