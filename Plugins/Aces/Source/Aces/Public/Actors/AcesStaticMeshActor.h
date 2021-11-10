// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "AcesStaticMeshActor.generated.h"

UCLASS(HideDropdown, NotBlueprintable, NotBlueprintType, meta = (ChildCannotTick, IsBlueprintBase="false"))
class ACES_API AAcesStaticMeshActor final : public AStaticMeshActor
{
	GENERATED_UCLASS_BODY()
};
