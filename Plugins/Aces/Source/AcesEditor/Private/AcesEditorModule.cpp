// Copyright 2021 Andrew Macdonald

#include "AcesEditor/Public/AcesEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

IMPLEMENT_GAME_MODULE(FAcesEditorModule, AcesEditor);

DEFINE_LOG_CATEGORY(AcesEditor);
 
#define LOCTEXT_NAMESPACE "AcesEditor"
 
void FAcesEditorModule::StartupModule()
{
	UE_LOG(AcesEditor, Warning, TEXT("AcesEditor module has started!"));

	ComponentTypePinFactory = MakeShared<FComponentTypePinFactory>();
	FEdGraphUtilities::RegisterVisualPinFactory( ComponentTypePinFactory );
}
 
void FAcesEditorModule::ShutdownModule()
{
	UE_LOG(AcesEditor, Warning, TEXT("AcesEditor module has shut down"));
	FEdGraphUtilities::UnregisterVisualPinFactory(ComponentTypePinFactory);
}
 
#undef LOCTEXT_NAMESPACE
 

// Add default functionality here for any IUnrealFlecsEditorModule functions that are not pure virtual.
