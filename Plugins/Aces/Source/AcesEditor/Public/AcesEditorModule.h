// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "ComponentTypePinFactory.h"

DECLARE_LOG_CATEGORY_EXTERN( AcesEditor, All, All );

class FAcesEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FComponentTypePinFactory> ComponentTypePinFactory;
};