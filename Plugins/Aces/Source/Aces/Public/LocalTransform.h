// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "Component.h"
#include "Core/Public/Math/Transform.h"

#include "LocalTransform.generated.h"

USTRUCT( Blueprintable, BlueprintType )
struct ACES_API FLocalTransform : public FComponent
{
	GENERATED_BODY()

		UPROPERTY( BlueprintReadWrite, EditAnywhere )
		FTransform localTransform;
};
