#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EventRunTypes.h"
#include "GameplayTagContainer.h"
#include "EventRunSubsystem.generated.h"

class UEventDatabase;
class UPlanetDefinition;
class UEncounterDefinition;
class UShipStateComponent;

USTRUCT()
struct FRunNode
{
	GENERATED_BODY()

	UPROPERTY() TObjectPtr<const UPlanetDefinition> PlanetA = nullptr;
	UPROPERTY() TObjectPtr<const UPlanetDefinition> PlanetB = nullptr;

	// Encounter based on planet biome
	UPROPERTY() TObjectPtr<const UEncounterDefinition> EncounterA = nullptr;
	UPROPERTY() TObjectPtr<const UEncounterDefinition> EncounterB = nullptr;

	// Locked outcomes after scouting
	UPROPERTY() EOutcome LockedA = EOutcome::Unknown;
	UPROPERTY() EOutcome LockedB = EOutcome::Unknown;

	// Optional: store what skip encounter rolled this step
	UPROPERTY() TObjectPtr<const UEncounterDefinition> SkipEncounter = nullptr;
};

UCLASS(BlueprintType)
class TERMINALSHUTDOWN_API UEventRunSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Configurable balancing parameters
	UPROPERTY(EditAnywhere, Category = "Balancing") float K = 1.0f;         // adaptation strength
	UPROPERTY(EditAnywhere, Category = "Balancing") float WDmg = 0.6f;      // weight damage
	UPROPERTY(EditAnywhere, Category = "Balancing") float WEnergy = 0.4f;   // weight energy
	UPROPERTY(EditAnywhere, Category = "Balancing") float STarget = 0.5f;   // target tension
	UPROPERTY(EditAnywhere, Category = "Balancing") int32 MaxEnergy = 20;

	// Costs
	UPROPERTY(EditAnywhere, Category = "Costs") int32 CostSkip = 1;
	UPROPERTY(EditAnywhere, Category = "Costs") int32 CostScout = 1;
	UPROPERTY(EditAnywhere, Category = "Costs") int32 CostLand = 2;

	// Skip behavior
	UPROPERTY(EditAnywhere, Category = "Skip") float SkipEncounterChance = 0.4f; // chance an encounter happens on skip
	UPROPERTY(EditAnywhere, Category = "Skip") float SkipBasePn = 0.65f;          // if skip encounter happens, base negative chance

	// Runtime API
	UFUNCTION(BlueprintCallable) void StartRun(int32 Seed);
	UFUNCTION(BlueprintCallable) FRunNodeView GetCurrentNodeView(UShipStateComponent* Ship) const;

	UFUNCTION(BlueprintCallable) bool Scout(UShipStateComponent* Ship, EPlanetSide Side, TArray<FText>& OutLogs);
	UFUNCTION(BlueprintCallable) bool Choose(UShipStateComponent* Ship, ERunChoice Choice, TArray<FText>& OutLogs);
	UFUNCTION(BlueprintCallable) bool IsRunEnded() const { return bRunEnded; }
	UFUNCTION(BlueprintCallable) FText GetEndReason() const { return EndReason; }

private:
	UPROPERTY() TObjectPtr<UEventDatabase> DB = nullptr;
	UPROPERTY() FRunNode Current;
	UPROPERTY() bool bRunEnded = false;
	UPROPERTY() FText EndReason;
	FRandomStream Rng;

	void LoadDatabase();
	void GenerateCurrentNode();

	// Resolution helpers
	float ComputeAdjustedPn(float BasePn, const UShipStateComponent* Ship, float ActionMultiplier) const;
	EOutcome RollOutcome(float PnAdjusted, FRandomStream& Stream) const;

	void ApplyOutcome(const UEncounterDefinition* Encounter, EOutcome Outcome, UShipStateComponent* Ship, TArray<FText>& OutLogs) const;
	void ApplyRules(const TArray<struct FConditionalEffectRule>& Rules, const FGameplayTagContainer& Modules, UShipStateComponent* Ship, TArray<FText>& OutLogs) const;
};
