// Copyright 2021 Andrew Macdonald

#include "ComponentTypePinFactory.h"
#include "K2Node.h"
#include "EdGraphSchema_K2.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SGraphPin.h"
#include "SGraphPinComponentStruct.h"
#include "Component.h"

TSharedPtr<class SGraphPin> FComponentTypePinFactory::CreatePin( class UEdGraphPin* InPin ) const
{
	if( InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && InPin->Direction == EEdGraphPinDirection::EGPD_Input )
	{
		if( UScriptStruct* PinStructType = Cast<UScriptStruct>( InPin->PinType.PinSubCategoryObject.Get() ) )
		{
			if( PinStructType->IsChildOf( FComponent::StaticStruct() ) )
			{
				return SNew( SGraphPinComponentStruct, InPin );
			}
		}
	}

	return nullptr;
}
