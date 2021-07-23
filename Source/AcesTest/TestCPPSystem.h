// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "BaseSystem.h"

#include "TestCPPSystem.generated.h"

UCLASS()
class ACESTEST_API UTestCPPSystem : public UBaseSystem
{
	GENERATED_BODY()
	virtual void Tick_Implementation( UAcesSubsystem* AcesSubsystem, float deltaTime ) override;
};
