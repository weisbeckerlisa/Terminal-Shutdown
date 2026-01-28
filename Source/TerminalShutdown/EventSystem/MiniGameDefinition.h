// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "MiniGameDefinition.generated.h"


UCLASS(BlueprintType)
class UMiniGameDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MiniGameTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimeLimit = 30.0f;
}; 