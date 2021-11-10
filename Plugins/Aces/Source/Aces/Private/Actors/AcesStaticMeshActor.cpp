// Copyright 2021 Andrew Macdonald

#include "Actors/AcesStaticMeshActor.h"

// Sets default values
AAcesStaticMeshActor::AAcesStaticMeshActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{	
	SetCanBeDamaged(false);
	PrimaryActorTick.bCanEverTick = false;
}
