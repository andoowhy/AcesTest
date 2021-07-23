// Copyright 2021 Andrew Macdonald

#include "AcesModule.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

IMPLEMENT_GAME_MODULE(FAcesModule, Aces);

DEFINE_LOG_CATEGORY(Aces);
 
#define LOCTEXT_NAMESPACE "Aces"
 
void FAcesModule::StartupModule()
{
	UE_LOG(Aces, Warning, TEXT("Aces module has started!"));
}
 
void FAcesModule::ShutdownModule()
{
	UE_LOG(Aces, Warning, TEXT("Aces module has shut down"));
}
 
#undef LOCTEXT_NAMESPACE
 

// Add default functionality here for any IAcesModule functions that are not pure virtual.
