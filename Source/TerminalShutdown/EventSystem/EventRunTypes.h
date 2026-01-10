#pragma once
#include "CoreMinimal.h"
#include "EventRunTypes.generated.h"

UENUM(BlueprintType)
enum class EPlanetSide : uint8 { A, B };

UENUM(BlueprintType)
enum class ERunChoice : uint8 { PlanetA, Skip, PlanetB };

UENUM(BlueprintType)
enum class EOutcome : uint8 { Unknown, Positive, Negative };

USTRUCT(BlueprintType)
struct FRunOptionView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FText Label;
	UPROPERTY(BlueprintReadOnly) FText Preview;      // planet preview or skip preview
	UPROPERTY(BlueprintReadOnly) int32 EnergyCost = 0;
	UPROPERTY(BlueprintReadOnly) bool bCanScout = false;
	UPROPERTY(BlueprintReadOnly) bool bScouted = false;
	UPROPERTY(BlueprintReadOnly) FText ScoutInfo;   // filled after scouting
};

USTRUCT(BlueprintType)
struct FRunNodeView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FRunOptionView PlanetA;
	UPROPERTY(BlueprintReadOnly) FRunOptionView Skip;
	UPROPERTY(BlueprintReadOnly) FRunOptionView PlanetB;
};
