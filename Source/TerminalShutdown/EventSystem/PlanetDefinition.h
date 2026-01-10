#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EncounterDefinition.h"
#include "PlanetDefinition.generated.h"

UCLASS(BlueprintType)
class UPlanetDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText PlanetName;

	// General description shown as the planet preview (before scouting)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText PlanetPreviewDescription;

	// Base probability of Negative outcome on this planet (before dynamic adjustment)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BasePn = 0.4f;

	// Points to a paired encounter (positive/negative variants)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UEncounterDefinition> Encounter;

	// Optional: for generator filtering
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer PlanetTags;
};
