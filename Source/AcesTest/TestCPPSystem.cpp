// Copyright 2021 Andrew Macdonald

#include "TestCPPSystem.h"

#include "AcesSubsystem.h"

#include "LocalTransform.h"
#include "WorldTransform.h"

void UTestCPPSystem::Tick_Implementation( UAcesSubsystem* AcesSubsystem, float deltaTime )
{
	AcesSubsystem->Each<FLocalTransform, FWorldTransform>([&](auto* LocalTransform, auto* WorldTransform)
	{

	});
}
