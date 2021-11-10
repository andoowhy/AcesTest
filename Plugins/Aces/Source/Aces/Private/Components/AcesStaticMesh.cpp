// Copyright 2021 Andrew Macdonald

#include "Components/AcesStaticMesh.h"

UStaticMesh* FAcesStaticMesh::GetStaticMesh() const
{
	return HelperActor->GetStaticMeshComponent()->GetStaticMesh();
}

void FAcesStaticMesh::SetStaticMesh(UStaticMesh* StaticMesh) const
{
	HelperActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
}

void FAcesStaticMesh::CreateHelperActor()
{
	HelperActor = NewObject<AAcesStaticMeshActor>();
}

void FAcesStaticMesh::DestroyHelperActor() const
{
	HelperActor->BeginDestroy();
}
