// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "BaseSystem.generated.h"

UCLASS(abstract)
class ACES_API UBaseSystem : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION( BlueprintNativeEvent, Category = "Aces" )
	void Tick( UAcesSubsystem* AcesSubsystem, float deltaTime );
	virtual void Tick_Implementation( UAcesSubsystem* AcesSubsystem, float deltaTime );
};
