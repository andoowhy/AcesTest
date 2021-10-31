// Copyright 2021 Andrew Macdonald

#include "TestCPPSystem.h"

#include "AcesSubsystem.h"

#include "Components/LocalTransform.h"
#include "Components/WorldTransform.h"

void UTestCPPSystem::Tick_Implementation( UAcesSubsystem* AcesSubsystem, float deltaTime )
{
	AcesSubsystem->Each<FLocalTransform, FWorldTransform>( [&]( auto Entity, auto* LocalTransform, auto* WorldTransform )
	{
		LocalTransform->LocalTransform.AddToTranslation( FVector( deltaTime, 0.0, 0.0 ) );
		UE_LOG( LogTemp, Warning, TEXT( "%s" ), *LocalTransform->LocalTransform.ToString() );

		if (LocalTransform->LocalTransform.GetTranslation().X > 5.0f)
		{
			AcesSubsystem->DestroyEntity(Entity);
		}
	} );
}
