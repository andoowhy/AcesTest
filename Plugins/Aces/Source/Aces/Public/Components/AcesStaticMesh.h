// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "Component.h"
#include "Actors/AcesStaticMeshActor.h"

#include "AcesStaticMesh.generated.h"

USTRUCT( Blueprintable, BlueprintType )
struct ACES_API FAcesStaticMesh : public FComponent
{
	GENERATED_BODY()

	UStaticMesh* GetStaticMesh() const;
	void SetStaticMesh(UStaticMesh* StaticMesh) const;

	friend class UAcesSubsystem;
private:
	AAcesStaticMeshActor* HelperActor;
	void CreateHelperActor();
	void DestroyHelperActor() const;
};
