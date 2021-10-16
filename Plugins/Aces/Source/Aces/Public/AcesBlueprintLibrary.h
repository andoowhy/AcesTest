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

	UFUNCTION( BlueprintCallable, CustomThunk, meta = ( BlueprintInternalUseOnly="true", CustomStructureParam = "OutComponent") )
	static bool GetComponentData( const FEntityHandle Entity, const int32 ComponentArrayIndex, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles, FComponent& OutComponent );
	
	static bool Generic_GetComponentData( const FEntityHandle Entity, const int32 ComponentArrayIndex, const TArray<UComponentSparseArrayHandle*> MatchingComponentArrayHandles, void* OutComponentPtr );

	/** Based on UAcesBlueprintLibrary::GetComponentData */
	DECLARE_FUNCTION( execGetComponentData )
	{
		P_GET_STRUCT(FEntityHandle, Entity);
		P_GET_PROPERTY(FIntProperty, ComponentArrayIndex);
		P_GET_TARRAY(UComponentSparseArrayHandle*, MatchingComponentArrayHandles);

		Stack.StepCompiledIn<FStructProperty>( NULL );
		void* OutComponentPtr = Stack.MostRecentPropertyAddress;

		P_FINISH;
		bool bSuccess = false;

		FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
		
		UScriptStruct* OutputType = StructProp->Struct;
		UScriptStruct* ComponentStruct = MatchingComponentArrayHandles[ComponentArrayIndex]->GetComponentSparseArray()->GetComponentStruct();

		if( StructProp && OutComponentPtr )
		{
			const bool bCompatible =
				( OutputType == ComponentStruct ) ||
				( OutputType->IsChildOf( ComponentStruct ) && FStructUtils::TheSameLayout( OutputType, ComponentStruct ) );	

			if( bCompatible )
			{
				P_NATIVE_BEGIN;
				bSuccess = Generic_GetComponentData( Entity, ComponentArrayIndex, MatchingComponentArrayHandles, OutComponentPtr );
				P_NATIVE_END;
			} else
			{
				FBlueprintExceptionInfo ExceptionInfo(
					EBlueprintExceptionType::AccessViolation,
					NSLOCTEXT( "GetComponentData", "IncompatibleProperty", "Incompatible output parameter; the Component's type is not the same as the return type." )
				);
				FBlueprintCoreDelegates::ThrowScriptException( P_THIS, Stack, ExceptionInfo );
			}
		}
		else
		{
			bSuccess = false;
		}

		*(bool*)RESULT_PARAM = bSuccess;
	}

	UFUNCTION( BlueprintCallable, meta = ( BlueprintInternalUseOnly ) )
	static void IterAdvance( UComponentSparseArrayHandle* const ComponentSparseArrayHandle );

	UFUNCTION( BlueprintCallable, meta = ( BlueprintInternalUseOnly ) )
	static bool IterIsValid( UComponentSparseArrayHandle* const ComponentSparseArrayHandle );

	UFUNCTION( BlueprintCallable, meta = ( BlueprintInternalUseOnly ) )
	static FEntityHandle IterGetEntity( UComponentSparseArrayHandle* const ComponentSparseArrayHandle );
};