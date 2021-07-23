// Copyright 2021 Andrew Macdonald

#pragma once

#include "CoreMinimal.h"

class UAcesSubsystem;

class ACES_API AcesQuery
{
	friend class UAcesSubsystem;

private:
	UAcesSubsystem* Aces;
	TArray<TTuple<SIZE_T, FScriptSparseArrayLayout>> ComponentTypeInfos;
};
