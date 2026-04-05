#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EventEffects.generated.h"

USTRUCT(BlueprintType)
struct FEventEffect
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 EnergyDelta = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DamageDelta = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 FoodDelta = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 WaterDelta = 0;

	// For later: status effects, module changes, etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer AddTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer RemoveTags;
};

USTRUCT(BlueprintType)
struct FConditionalEffectRule
{
	GENERATED_BODY()

	// If empty then always true
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery Condition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEventEffect Effect;

	// Extra log lines if this rule triggers
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FText> ExtraLogs;
};

USTRUCT(BlueprintType)
struct FOutcomeDefinition
{
	GENERATED_BODY()

	// Shown after scouting (precise)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ScoutDescription;

	// Shown when the outcome happens
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FText> ResolveLogs;

	// Rules evaluated at runtime based on ship active module tags
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FConditionalEffectRule> ConditionalRules;
};
