// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "ComponentSparseArray.h"
#include "ComponentSparseArrayHandle.h"
#include "ComponentSparseArrayIterHandle.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "AcesBlueprintLibrary.generated.h"

class UAcesSubsystem;

UCLASS(BlueprintInternalUseOnly)
class ACES_API UAcesBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
friend class UK2Node_AcesQuery;

	UFUNCTION(meta=(BlueprintInternalUseOnly))
	static UComponentSparseArrayHandle* GetSmallestMatchingComponentArrayHandle(UAcesSubsystem* Aces, TArray<uint32> MatchingComponentArrayIndices);

	UFUNCTION(meta=(BlueprintInternalUseOnly))
	static TArray<UComponentSparseArrayHandle*> GetMatchingComponentArrays( UAcesSubsystem* Aces, TArray<UScriptStruct*> ComponentScriptStructs );
};
