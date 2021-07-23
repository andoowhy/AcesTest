// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"

#include "Component.generated.h"

USTRUCT( BlueprintInternalUseOnly )
struct ACES_API FComponent
{
	GENERATED_USTRUCT_BODY()

	FComponent() {}
	virtual ~FComponent() {}
};
