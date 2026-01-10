#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlanetDefinition.h"
#include "EncounterDefinition.h"
#include "EventDatabase.generated.h"

UCLASS(BlueprintType)
class UEventDatabase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UPlanetDefinition>> Planets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UEncounterDefinition>> PlanetEncounters;

	// Skip hazards
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UEncounterDefinition>> TravelEncounters;
};
