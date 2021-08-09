// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "UObject/ObjectMacros.h"
#include "K2Node_AddPinInterface.h"
#include "EdGraph/EdGraphPin.h"
#include "Textures/SlateIcon.h"
#include "K2Node_AcesQuery.generated.h"

class FBlueprintActionDatabaseRegistrar;
class UEdGraph;

UCLASS( MinimalAPI )
class UK2Node_AcesQuery : public UK2Node, public IK2Node_AddPinInterface
{
	GENERATED_UCLASS_BODY()

	static const FName AcesPinName;
	static const FName TickPinName;
	static const FName EachPinName;

	//~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle( ENodeTitleType::Type TitleType ) const override;
	virtual void GetNodeContextMenuActions( class UToolMenu* menu, class UGraphNodeContextMenuContext* contex ) const override;
	//virtual bool IncludeParentNodeContextMenu() const override { return true; }
	virtual void PinDefaultValueChanged( UEdGraphPin* Pin ) override;
	//virtual void PinTypeChanged( UEdGraphPin* Pin ) override;
	//virtual FText GetTooltipText() const override;
	//virtual FSlateIcon GetIconAndTint( FLinearColor& OutColor ) const override;
	//~ End UEdGraphNode Interface.

	//~ Begin UK2Node Interface.
	virtual void ExpandNode( class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph ) override;
	virtual void NotifyPinConnectionListChanged( UEdGraphPin* Pin ) override;
	//virtual void PostReconstructNode() override;
	//virtual bool IsConnectionDisallowed( const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason ) const override;
	virtual void GetMenuActions( FBlueprintActionDatabaseRegistrar& ActionRegistrar ) const override;
	virtual FText GetMenuCategory() const override;
	virtual bool IsNodePure() const override
	{
		return false;
	}
	//~ End UK2Node Interface.

	// IK2Node_AddPinInterface interface
	virtual void AddInputPin() override;
	FName GetUniquePinName( EEdGraphPinDirection direction );
	FName GetPinName( int32 index, EEdGraphPinDirection direction ) const;
	virtual void RemoveInputPin( UEdGraphPin* Pin ) override;
	// End of IK2Node_AddPinInterface interface

	bool CheckForErrors( FKismetCompilerContext& CompilerContext ) const;
	void InteractiveAddInputPin();
	UEdGraphPin* GetOppositePin( UEdGraphPin* pin ) const;
	bool IsComponentClassInputPin( UEdGraphPin* Pin ) const;
	bool IsComponentObjectOutputPin( UEdGraphPin* Pin ) const;
	TArray<TTuple<UEdGraphPin*,UEdGraphPin*>> GetComponentInputOutputPins() const;
	void ResetInputPin( UEdGraphPin* Pin ) const;
};
