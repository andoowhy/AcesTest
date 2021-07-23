// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "BlueprintAcesSystem.generated.h"

UCLASS(abstract, BlueprintType, Blueprintable, Category="Aces")
class ACES_API UBlueprintAcesSystem: public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Aces")
	TArray<TSubclassOf<USceneComponent>> classes;

	UFUNCTION( BlueprintNativeEvent, Category = "Aces" )
	void Tick(UAcesSubsystem* aces, float deltaTime);
	virtual void Tick_Implementation(UAcesSubsystem* aces, float deltaTime);
};
