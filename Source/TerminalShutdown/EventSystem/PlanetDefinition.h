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

	// Biome filter
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag BiomeTag;

	// Image shown in UI for this planet
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> PlanetImage;
};
