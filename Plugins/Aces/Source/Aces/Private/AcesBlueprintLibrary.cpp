// Copyright 2021 Andrew Macdonald

#include "AcesBlueprintLibrary.h"
#include "AcesSubsystem.h"

FORCEINLINE UComponentSparseArrayHandle* UAcesBlueprintLibrary::GetSmallestMatchingComponentArrayHandle(UAcesSubsystem* Aces, TArray<uint32> MatchingComponentArrayIndices)
{
	return Aces->GetSmallestMatchingComponentArrayHandle(MatchingComponentArrayIndices);
}

FORCEINLINE TArray<UComponentSparseArrayHandle*> UAcesBlueprintLibrary::GetMatchingComponentArrays(UAcesSubsystem* Aces, TArray<UScriptStruct*> ComponentScriptStructs )
{
	return Aces->GetMatchingComponentArrays(ComponentScriptStructs);
}
