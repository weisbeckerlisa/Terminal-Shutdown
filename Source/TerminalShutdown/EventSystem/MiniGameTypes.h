#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EventRunTypes.h"
#include "MiniGameTypes.generated.h"

class UMiniGameDefinition;

UENUM(BlueprintType)
enum class EMiniGameResult : uint8
{
	Success,
	Fail,
	Timeout
};

USTRUCT(BlueprintType)
struct FMiniGameContext
{
	GENERATED_BODY()

	// Which mini game is running
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMiniGameDefinition> Definition = nullptr;

	// Time limit
	UPROPERTY(BlueprintReadOnly)
	float TimeLimit = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float TimeRemaining = 0.f;

	// Identify where it came from
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag EncounterTag;

	// Which planet side triggered it
	UPROPERTY(BlueprintReadOnly)
	EPlanetSide Side = EPlanetSide::A;
};
