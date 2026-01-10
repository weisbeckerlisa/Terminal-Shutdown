#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagContainer.h"
#include "EventEffects.generated.h"

USTRUCT(BlueprintType)
struct FEventEffect
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 EnergyDelta = 0;

	// +1 means take 1 damage. You cap at 3 (game over handled elsewhere).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DamageDelta = 0;

	// Optional: allow tagging persistent states (e.g., "Ship.Status.Leaking")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer AddTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer RemoveTags;
};

USTRUCT(BlueprintType)
struct FConditionalEffectRule
{
	GENERATED_BODY()

	// If empty, considered "always true"
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery Condition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEventEffect Effect;

	// Optional extra log lines if this rule triggers
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
