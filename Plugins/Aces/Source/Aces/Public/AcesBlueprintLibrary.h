// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Component.h"
#include "ComponentSparseArrayHandle.h"
#include "AcesSubsystem.h"

#include "AcesBlueprintLibrary.generated.h"

class UAcesSubsystem;
class UComponentSparseArrayHandle;

UCLASS( BlueprintInternalUseOnly )
class ACES_API UAcesBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	friend class UK2Node_AcesQuery;

	UFUNCTION( meta = ( BlueprintInternalUseOnly ) )
	static TArray<UComponentSparseArrayHandle*> GetMatchingComponentArrayHandles( UAcesSubsystem* const Aces, const TArray<UScriptStruct*> ComponentScriptStructs );

	UFUNCTION( meta = ( BlueprintInternalUseOnly ) )
	static UComponentSparseArrayHandle* GetSmallestMatchingComponentArrayHandle( UAcesSubsystem* const Aces, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles );

	UFUNCTION( meta = ( BlueprintInternalUseOnly ) )
	static bool IsEntityInAllComponentArrayHandles( UAcesSubsystem* const Aces, const uint32 Entity, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles );

	UFUNCTION( meta = ( BlueprintInternalUseOnly ) )
	static FComponent& GetComponentData( const uint32 Entity, UComponentSparseArrayHandle* const ComponentSparseArrayHandle );

	UFUNCTION( meta = ( BlueprintInternalUseOnly ) )
	static void IterAdvance( UComponentSparseArrayHandle* const ComponentSparseArrayHandle );

	UFUNCTION( meta = ( BlueprintInternalUseOnly ) )
	static bool IterIsValid( UComponentSparseArrayHandle* const ComponentSparseArrayHandle );

	UFUNCTION( meta = ( BlueprintInternalUseOnly ) )
	static uint32 IterGetEntity( UComponentSparseArrayHandle* const ComponentSparseArrayHandle );

};