// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "Component.h"
#include "ComponentSparseArray.h"

#include "ComponentSparseArrayHandle.generated.h"

UCLASS(BlueprintInternalUseOnly)
class ACES_API UComponentSparseArrayHandle : public UObject
{
	GENERATED_BODY()
public:
	UComponentSparseArrayHandle* Init(TComponentSparseArray* InComponentSparseArray);

	TComponentSparseArray* GetComponentSparseArray() { return ComponentSparseArray; }

	FORCEINLINE FComponent& GetComponentData( const uint32 Entity );

	UFUNCTION(meta = ( BlueprintInternalUseOnly ))
	void IterAdvance();

	UFUNCTION( meta = ( BlueprintInternalUseOnly ) )
	bool IterIsValid();

	UFUNCTION( meta = ( BlueprintInternalUseOnly ) )
	uint32 IterGetEntity();

	void BeginDestroy() override;

private:
	TComponentSparseArray* ComponentSparseArray;
	TComponentSparseArray::TComponentSparseArrayIterator* Iterator;
};
