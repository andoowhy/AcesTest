// Copyright 2021 Andrew Macdonald

#pragma once

#include "EntityHandle.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct ACES_API FEntityHandle
{
	GENERATED_BODY()
	
	FEntityHandle()
	{
		Entity = UINT32_MAX;
	}

	uint32 Entity;
};
