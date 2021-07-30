// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"

#include "ComponentSparseArrayHandle.generated.h"

UCLASS(BlueprintInternalUseOnly)
class ACES_API UComponentSparseArrayHandle : public UObject
{
	GENERATED_BODY()
public:
	UComponentSparseArrayHandle* Init(uint32 InComponentArrayIndex)
	{
		ComponentArrayIndex = InComponentArrayIndex;
		return this;
	}

private:
	uint32 ComponentArrayIndex;
};
