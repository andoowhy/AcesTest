// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Component.h"
#include "ComponentSparseArrayHandle.h"
#include "AcesSubsystem.h"

#include "AcesBlueprintLibrary.generated.h"

class UAcesSubsystem;
struct FEntityHandle;
class UComponentSparseArrayHandle;

UCLASS( BlueprintInternalUseOnly )
class ACES_API UAcesBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	friend class UK2Node_AcesQuery;

	UFUNCTION( BlueprintCallable, meta = ( BlueprintInternalUseOnly ) )
	static TArray<UComponentSparseArrayHandle*> GetMatchingComponentArrayHandles( UAcesSubsystem* const Aces, const TArray<UScriptStruct*> ComponentScriptStructs );

	UFUNCTION( BlueprintCallable, meta = ( BlueprintInternalUseOnly ) )
	static UComponentSparseArrayHandle* GetSmallestMatchingComponentArrayHandle( UAcesSubsystem* const Aces, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles );

	UFUNCTION( BlueprintCallable, meta = ( BlueprintInternalUseOnly ) )
	static bool IsEntityInAllComponentArrayHandles( UAcesSubsystem* const Aces, const FEntityHandle Entity, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles );

	UFUNCTION( BlueprintCallable, meta = ( BlueprintInternalUseOnly ) )
	static FComponent& GetComponentData( const FEntityHandle Entity, const int32 ComponentArrayIndex, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles );

	UFUNCTION( BlueprintCallable, meta = ( BlueprintInternalUseOnly ) )
	static void IterAdvance( UComponentSparseArrayHandle* const ComponentSparseArrayHandle );

	UFUNCTION( BlueprintCallable, meta = ( BlueprintInternalUseOnly ) )
	static bool IterIsValid( UComponentSparseArrayHandle* const ComponentSparseArrayHandle );

	UFUNCTION( BlueprintCallable, meta = ( BlueprintInternalUseOnly ) )
	static FEntityHandle IterGetEntity( UComponentSparseArrayHandle* const ComponentSparseArrayHandle );
};