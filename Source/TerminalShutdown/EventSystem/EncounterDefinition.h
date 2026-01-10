#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EventEffects.h"
#include "EncounterDefinition.generated.h"

// One "theme" with 2 complementary outcomes: Positive and Negative.
UCLASS(BlueprintType)
class UEncounterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// For filtering / analytics / balancing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag EncounterTag;

	// General hint shown BEFORE scouting/landing (planet preview)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText GeneralHint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FOutcomeDefinition Positive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FOutcomeDefinition Negative;
};
