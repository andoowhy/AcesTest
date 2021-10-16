// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "Component.h"
#include "Core/Public/Math/Transform.h"

#include "WorldTransform.generated.h"

USTRUCT( Blueprintable, BlueprintType )
struct ACES_API FWorldTransform : public FComponent
{
	GENERATED_BODY()

	UPROPERTY( BlueprintReadWrite, EditAnywhere )
	FTransform WorldTransform;
};
