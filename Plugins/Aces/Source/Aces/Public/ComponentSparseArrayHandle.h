// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "Component.h"
#include "ComponentSparseArray.h"

#include "ComponentSparseArrayHandle.generated.h"

UCLASS(BlueprintInternalUseOnly)
class ACES_API UComponentSparseArrayHandle : public UObject
{
	GENERATED_BODY()
public:
	UComponentSparseArrayHandle* Init(TComponentSparseArray* InComponentSparseArray)
	{
		ComponentSparseArray = InComponentSparseArray;
		return this;
	}

	FORCEINLINE TComponentSparseArray* GetComponentSparseArray() { return ComponentSparseArray; }

private:
	TComponentSparseArray* ComponentSparseArray;
};
