// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"

class SGraphPin;

class ACESEDITOR_API FComponentTypePinFactory : public FGraphPanelPinFactory
{
	//~ Begin FGraphPanelPinFactory implementation
	virtual TSharedPtr<class SGraphPin> CreatePin( class UEdGraphPin* InPin ) const override;
	//~ End FGraphPanelPinFactory implementation
};
