// Copyright 2021 Andrew Macdonald


#include "K2Node_AcesQuery.h"
#include "EdGraph/EdGraphPin.h"
#include "Engine/Blueprint.h"
#include "Framework/Commands/UIAction.h"
#include "ToolMenus.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "ScopedTransaction.h"
#include "EdGraphUtilities.h"
#include "KismetCompiledFunctionContext.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "Kismet/KismetMathLibrary.h"

#include "K2Node_MakeArray.h"
#include "K2Node_CallFunction.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_TemporaryVariable.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_ExecutionSequence.h"

#include "AcesBlueprintLibrary.h"
#include "Aces/Public/AcesSubsystem.h"
#include "Aces/Public/Component.h"
#include "ComponentSparseArray.h"
#include "ComponentSparseArrayHandle.h"

#define LOCTEXT_NAMESPACE "K2Node_AcesQuery"

struct FQueryExpandNodeHelper
{
	FQueryExpandNodeHelper()
	{
	}
};

UK2Node_AcesQuery::UK2Node_AcesQuery( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
}

const FName UK2Node_AcesQuery::AcesPinName( TEXT( "Aces" ) );
const FName UK2Node_AcesQuery::TickPinName( TEXT( "Tick" ) );
const FName UK2Node_AcesQuery::EachPinName( TEXT( "Each" ) );

void UK2Node_AcesQuery::AllocateDefaultPins()
{
	CreatePin( EGPD_Input, UEdGraphSchema_K2::PC_Exec, TickPinName );
	CreatePin( EGPD_Input, UEdGraphSchema_K2::PC_Object, UAcesSubsystem::StaticClass(), AcesPinName );

	CreatePin( EGPD_Output, UEdGraphSchema_K2::PC_Exec, EachPinName );

	Super::AllocateDefaultPins();
}

FText UK2Node_AcesQuery::GetNodeTitle( ENodeTitleType::Type TitleType ) const
{
	return LOCTEXT( "AcesQuery_Title", "Aces Query" );
}

void UK2Node_AcesQuery::GetNodeContextMenuActions( class UToolMenu* menu, class UGraphNodeContextMenuContext* context ) const
{
	Super::GetNodeContextMenuActions( menu, context );

	if( context->bIsDebugging )
	{
		return;
	}

	FToolMenuSection& section = menu->AddSection( "K2NodeAcesQuery", NSLOCTEXT( "K2Nodes", "AcesQueryHeader", "Aces Query" ) );

	auto pin = context->Pin;
	if( pin != NULL )
	{
		auto pinCategory = pin->PinType.PinCategory;
		if( pinCategory == UEdGraphSchema_K2::PC_String && pin->ParentPin == nullptr )
		{
			section.AddMenuEntry(
				"RemovePin",
				LOCTEXT( "AcesQuery_RemovePin", "Remove component pin pair" ),
				LOCTEXT( "AcesQuery_RemovePinTooltip", "Remove this component's class selection and corresponding output pins" ),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateUObject( const_cast<UK2Node_AcesQuery*>( this ), &UK2Node_AcesQuery::RemoveInputPin, const_cast<UEdGraphPin*>( pin ) )
				)
			);
		}
	} else
	{
		section.AddMenuEntry(
			"AddPin",
			LOCTEXT( "AddPin", "Add component pin pair" ),
			LOCTEXT( "AddPinTooltip", "Add another array element pin" ),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateUObject( const_cast<UK2Node_AcesQuery*>( this ), &UK2Node_AcesQuery::InteractiveAddInputPin )
			)
		);
	}
}

void UK2Node_AcesQuery::ExpandNode( class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph )
{
	Super::ExpandNode( CompilerContext, SourceGraph );

	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	check( Schema );

	UEdGraphPin* AcesPin = FindPin( AcesPinName )->LinkedTo[0];
	if( AcesPin == nullptr )
	{
		CompilerContext.MessageLog.Error( *NSLOCTEXT( "K2Node", "AcesQuerey_MissingAcesConnectionError", "Missing Aces Connection in @@" ).ToString(), this );
	}

	TArray<TTuple<UEdGraphPin*, UEdGraphPin*>> QueryComponentPins = GetComponentInputOutputPins();

	bool bResult = true;

	// Make Nodes
	UK2Node_MakeArray* MakeComponentScriptStructArray = CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>( this, SourceGraph );
	MakeComponentScriptStructArray->AllocateDefaultPins();

	UK2Node_MakeArray* MakeComponentReferencesArray = CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>( this, SourceGraph );
	MakeComponentReferencesArray->AllocateDefaultPins();

	UK2Node_CallFunction* CallFunctionGetMatchingComponentArrayHandles = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>( this, SourceGraph );
	CallFunctionGetMatchingComponentArrayHandles->SetFromFunction( UAcesBlueprintLibrary::StaticClass()->FindFunctionByName( GET_FUNCTION_NAME_CHECKED( UAcesBlueprintLibrary, GetMatchingComponentArrayHandles ) ) );
	CallFunctionGetMatchingComponentArrayHandles->AllocateDefaultPins();

	UK2Node_CallFunction* CallFunctionGetSmallestMatchingComponentArrayHandle = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>( this, SourceGraph );
	CallFunctionGetSmallestMatchingComponentArrayHandle->SetFromFunction( UAcesBlueprintLibrary::StaticClass()->FindFunctionByName( GET_FUNCTION_NAME_CHECKED( UAcesBlueprintLibrary, GetSmallestMatchingComponentArrayHandle ) ) );
	CallFunctionGetSmallestMatchingComponentArrayHandle->AllocateDefaultPins();

	UK2Node_CallFunction* CallFunctionIterIsValid = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>( this, SourceGraph );
	CallFunctionIterIsValid->SetFromFunction( UAcesBlueprintLibrary::StaticClass()->FindFunctionByName( GET_FUNCTION_NAME_CHECKED( UAcesBlueprintLibrary, IterIsValid ) ) );
	CallFunctionIterIsValid->AllocateDefaultPins();

	UK2Node_CallFunction* CallFunctionIterAdvance = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>( this, SourceGraph );
	CallFunctionIterAdvance->SetFromFunction( UAcesBlueprintLibrary::StaticClass()->FindFunctionByName( GET_FUNCTION_NAME_CHECKED( UAcesBlueprintLibrary, IterAdvance ) ) );
	CallFunctionIterAdvance->AllocateDefaultPins();

	UK2Node_CallFunction* CallFunctionIterGetEntity = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>( this, SourceGraph );
	CallFunctionIterGetEntity->SetFromFunction( UAcesBlueprintLibrary::StaticClass()->FindFunctionByName( GET_FUNCTION_NAME_CHECKED( UAcesBlueprintLibrary, IterGetEntity ) ) );
	CallFunctionIterGetEntity->AllocateDefaultPins();
	
	UK2Node_CallFunction* CallFunctionIsEntityInAllComponentArrayHandles= CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>( this, SourceGraph );
	CallFunctionIsEntityInAllComponentArrayHandles->SetFromFunction( UAcesBlueprintLibrary::StaticClass()->FindFunctionByName( GET_FUNCTION_NAME_CHECKED( UAcesBlueprintLibrary, IsEntityInAllComponentArrayHandles ) ) );
	CallFunctionIsEntityInAllComponentArrayHandles->AllocateDefaultPins();

	UK2Node_IfThenElse* SmallestMatchingComponentLoopBranch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>( this, SourceGraph );
	SmallestMatchingComponentLoopBranch->AllocateDefaultPins();

	UK2Node_ExecutionSequence* IterExecutionSequence = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>( this, SourceGraph );
	IterExecutionSequence->AllocateDefaultPins();

	UK2Node_CallFunction* Condition = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>( this, SourceGraph );
	Condition->SetFromFunction( UKismetMathLibrary::StaticClass()->FindFunctionByName( GET_FUNCTION_NAME_CHECKED( UKismetMathLibrary, Less_IntInt ) ) );
	Condition->AllocateDefaultPins();

	// Copy Component Script Struct(s) from AcesQuery to new Array node
	for( SIZE_T Index = 0; Index < QueryComponentPins.Num(); ++Index )
	{
		UEdGraphPin* InputPin = QueryComponentPins[Index].Get<0>();
		check( InputPin );

		MakeComponentScriptStructArray->AddInputPin();
		// Add one to the index for the pin to set the default on to skip the output pin
		MakeComponentScriptStructArray->Pins[Index + 1]->DefaultObject = InputPin->DefaultObject;
	}

	// Get matching component arrays
	bResult &= Schema->TryCreateConnection( AcesPin,                                        CallFunctionGetMatchingComponentArrayHandles->FindPin( TEXT( "Aces" ) ) );
	bResult &= Schema->TryCreateConnection( MakeComponentScriptStructArray->GetOutputPin(), CallFunctionGetMatchingComponentArrayHandles->FindPin( TEXT( "ComponentScriptStructs" ) ) );
	
	// Get smallest component array
	bResult &= Schema->TryCreateConnection( AcesPin,                                                           CallFunctionGetSmallestMatchingComponentArrayHandle->FindPin( TEXT( "Aces" ) ) );
	bResult &= Schema->TryCreateConnection( CallFunctionGetMatchingComponentArrayHandles->GetReturnValuePin(), CallFunctionGetSmallestMatchingComponentArrayHandle->FindPin( TEXT( "MatchingComponentArrayHandles" ) ) );

	// Iterator Is Valid
	bResult &= Schema->TryCreateConnection( CallFunctionGetSmallestMatchingComponentArrayHandle->GetReturnValuePin(), CallFunctionIterIsValid->FindPin( TEXT( "ComponentSparseArrayHandle" ) ) );
	
	// Iterator Advance
	bResult &= Schema->TryCreateConnection( CallFunctionGetSmallestMatchingComponentArrayHandle->GetReturnValuePin(), CallFunctionIterAdvance->FindPin( TEXT( "ComponentSparseArrayHandle" ) ) );
	
	// Iterator Get Entity
	bResult &= Schema->TryCreateConnection( CallFunctionGetSmallestMatchingComponentArrayHandle->GetReturnValuePin(), CallFunctionIterGetEntity->FindPin( TEXT( "ComponentSparseArrayHandle" ) ) );
	
	// Is Entity In All ComponentArrayHandles
	bResult &= Schema->TryCreateConnection( AcesPin,                                                           CallFunctionIsEntityInAllComponentArrayHandles->FindPin( TEXT( "Aces" ) ) );
	bResult &= Schema->TryCreateConnection( CallFunctionIterGetEntity->GetReturnValuePin(),                    CallFunctionIsEntityInAllComponentArrayHandles->FindPin( TEXT( "Entity" ) ) );
	bResult &= Schema->TryCreateConnection( CallFunctionGetMatchingComponentArrayHandles->GetReturnValuePin(), CallFunctionIsEntityInAllComponentArrayHandles->FindPin( TEXT( "MatchingComponentArrayHandles" ) ) );
	
	BreakAllNodeLinks();
}

void UK2Node_AcesQuery::NotifyPinConnectionListChanged( UEdGraphPin* Pin )
{
	Super::NotifyPinConnectionListChanged( Pin );

	if( !IsComponentClassInputPin( Pin ) )
	{
		return;
	}

	ResetInputPin( Pin );
}

void UK2Node_AcesQuery::GetMenuActions( FBlueprintActionDatabaseRegistrar& ActionRegistrar ) const
{
	UClass* ActionKey = GetClass();
	if( ActionRegistrar.IsOpenForRegistration( ActionKey ) )
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create( GetClass() );
		check( NodeSpawner != nullptr );

		ActionRegistrar.AddBlueprintAction( ActionKey, NodeSpawner );
	}
}

FText UK2Node_AcesQuery::GetMenuCategory() const
{
	static FNodeTextCache CachedCategory;
	if( CachedCategory.IsOutOfDate( this ) )
	{
		// FText::Format() is slow, so we cache this to save on performance
		CachedCategory.SetCachedText( LOCTEXT( "AcesCategory", "Aces" ), this );
	}
	return CachedCategory;
}

void UK2Node_AcesQuery::AddInputPin()
{
	Modify();

	UEdGraphNode::FCreatePinParams InputPinParams;
	InputPinParams.bIsConst = true;

	UEdGraphPin* classPin = CreatePin( EGPD_Input, UEdGraphSchema_K2::PC_Struct, FComponent::StaticStruct(), GetUniquePinName( EGPD_Input ), InputPinParams );
	classPin->PinFriendlyName = LOCTEXT( "AcesCategory", "Component Class" );

	UEdGraphNode::FCreatePinParams OutputPinParams;
	OutputPinParams.bIsReference = true;

	UEdGraphPin* objectPin = CreatePin( EGPD_Output, UEdGraphSchema_K2::PC_Struct, FComponent::StaticStruct(), GetUniquePinName( EGPD_Output ), OutputPinParams );
	objectPin->PinFriendlyName = LOCTEXT( "AcesCategory", "Component" );
}

FName UK2Node_AcesQuery::GetUniquePinName( EEdGraphPinDirection direction )
{
	FName NewPinName;
	int32 i = 0;
	while( true )
	{
		NewPinName = GetPinName( i++, direction );
		if( !FindPin( NewPinName ) )
		{
			break;
		}
	}

	return NewPinName;
}

FName UK2Node_AcesQuery::GetPinName( int32 index, EEdGraphPinDirection direction ) const
{
	FString directionStr = StaticEnum<EEdGraphPinDirection>()->GetNameStringByValue( direction );
	return *FString::Printf( TEXT( "%s_%d_%s" ), TEXT( "Component" ), index, *directionStr );
}

void UK2Node_AcesQuery::RemoveInputPin( UEdGraphPin* pin )
{
	check( pin->ParentPin == nullptr );
	checkSlow( Pins.Contains( pin ) );

	UEdGraphPin* oppositePin = GetOppositePin( pin );
	/*check( oppositePin != nullptr );
	check( oppositePin->ParentPin == nullptr );*/

	FScopedTransaction Transaction( LOCTEXT( "AcesQuery_RemovePinTx", "AcesQuery_RemovePin" ) );
	Modify();

	TFunction<void( UEdGraphPin* )> removePinLambda = [this, &removePinLambda]( UEdGraphPin* PinToRemove )
	{
		for( int32 SubPinIndex = PinToRemove->SubPins.Num() - 1; SubPinIndex >= 0; --SubPinIndex )
		{
			removePinLambda( PinToRemove->SubPins[SubPinIndex] );
		}

		int32 PinRemovalIndex = INDEX_NONE;
		if( Pins.Find( PinToRemove, PinRemovalIndex ) )
		{
			Pins.RemoveAt( PinRemovalIndex );
			PinToRemove->MarkPendingKill();
		}
	};

	removePinLambda( pin );
	PinConnectionListChanged( pin );

	removePinLambda( oppositePin );
	PinConnectionListChanged( oppositePin );
	//SyncPinNames();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified( GetBlueprint() );
}

void UK2Node_AcesQuery::PinDefaultValueChanged( UEdGraphPin* Pin )
{
	Super::PinDefaultValueChanged( Pin );

	if( Pin->ParentPin != nullptr )
	{
		return;
	}

	if( !IsComponentClassInputPin( Pin ) )
	{
		return;
	}

	ResetInputPin( Pin );

	//RewireOldPinsToNewPins( OldClassPins, Pins, nullptr );
	GetGraph()->NotifyGraphChanged();
	FBlueprintEditorUtils::MarkBlueprintAsModified( GetBlueprint() );
}

bool UK2Node_AcesQuery::CheckForErrors( FKismetCompilerContext& CompilerContext ) const
{
	bool bError = false;



	return bError;
}

void UK2Node_AcesQuery::InteractiveAddInputPin()
{
	FScopedTransaction Transaction( LOCTEXT( "AddPinTx", "Add Pin" ) );
	AddInputPin();
}

UEdGraphPin* UK2Node_AcesQuery::GetOppositePin( UEdGraphPin* pin ) const
{
	EEdGraphPinDirection direction = pin->Direction;
	FString directionStr = StaticEnum<EEdGraphPinDirection>()->GetNameStringByValue( direction );

	EEdGraphPinDirection oppositeDirection = ( pin->Direction == EEdGraphPinDirection::EGPD_Input ) ? EEdGraphPinDirection::EGPD_Output : EEdGraphPinDirection::EGPD_Input;
	FString oppositeDirectionStr = StaticEnum<EEdGraphPinDirection>()->GetNameStringByValue( oppositeDirection );

	FString oppositePinName = pin->GetName();
	oppositePinName.RemoveFromEnd( directionStr );
	oppositePinName.Append( oppositeDirectionStr );

	return FindPin( oppositePinName );
}

bool UK2Node_AcesQuery::IsComponentClassInputPin( UEdGraphPin* Pin ) const
{
	return Pin->Direction == EEdGraphPinDirection::EGPD_Input && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct;
}

bool UK2Node_AcesQuery::IsComponentObjectOutputPin( UEdGraphPin* Pin ) const
{
	return Pin->Direction == EEdGraphPinDirection::EGPD_Output && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct;
}

TArray<TTuple<UEdGraphPin*, UEdGraphPin*>> UK2Node_AcesQuery::GetComponentInputOutputPins() const
{
	TArray<TTuple<UEdGraphPin*, UEdGraphPin*>> ComponentPins;
	for( auto Pin : Pins )
	{
		if( !IsComponentClassInputPin( Pin ) )
		{
			continue;
		}

		ComponentPins.Add( MakeTuple( Pin, GetOppositePin( Pin ) ) );
	}

	return MoveTempIfPossible( ComponentPins );
}

void UK2Node_AcesQuery::ResetInputPin( UEdGraphPin* Pin ) const
{
	UEdGraphPin* ObjectPin = GetOppositePin( Pin );
	if( Pin->LinkedTo.Num() == 0 )
	{
		if( Pin->DefaultObject )
		{
			ObjectPin->PinType.PinSubCategoryObject = Pin->DefaultObject;
			ObjectPin->PinFriendlyName = FText::FromName( ObjectPin->PinType.PinSubCategoryObject->GetFName() );
		} else
		{
			ObjectPin->PinType.PinSubCategoryObject = FComponent::StaticStruct();
			ObjectPin->PinFriendlyName = LOCTEXT( "AcesCategory", "Component" );
		}
	} else
	{
		UEdGraphPin* ClassSource = Pin->LinkedTo[0];
		if( ClassSource )
		{
			ObjectPin->PinType.PinSubCategoryObject = Pin->DefaultObject;
			ObjectPin->PinFriendlyName = FText::FromName( ObjectPin->PinType.PinSubCategoryObject->GetFName() );
		} else
		{
			ObjectPin->PinType.PinSubCategoryObject = FComponent::StaticStruct();
			ObjectPin->PinFriendlyName = LOCTEXT( "AcesCategory", "Component" );
		}
	}
}

#undef LOCTEXT_NAMESPACE