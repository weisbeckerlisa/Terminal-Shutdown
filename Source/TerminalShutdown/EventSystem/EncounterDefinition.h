#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EventEffects.h"
#include "MiniGameDefinition.h"
#include "EncounterDefinition.generated.h"

// One "theme" with 2 complementary outcomes: Positive and Negative.
UCLASS(BlueprintType)
class UEncounterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// Base probability of Negative outcome on this event (before dynamic adjustment)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BasePn = 0.4f;

	// Biome filter
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag BiomeTag;

	// General hint shown before scouting/landing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText GeneralHint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FOutcomeDefinition Positive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FOutcomeDefinition Negative;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMiniGameDefinition* MiniGame = nullptr;

};
