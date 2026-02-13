#pragma once

#include "CoreMinimal.h"
#include "UObject/ScriptDelegates.h"
#include "TimerManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EventRunTypes.h"
#include "GameplayTagContainer.h"
#include "MiniGameTypes.h"

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMiniGameStarted, const FMiniGameContext&, Context);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMiniGameEnded, EMiniGameResult, Result, const TArray<FText>&, Logs);

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
	UPROPERTY(EditAnywhere, Category = "Balancing") int32 MaxFoodUnits = 16;
	UPROPERTY(EditAnywhere, Category = "Balancing") int32 MaxWaterUnits = 16;

	// Costs
	UPROPERTY(EditAnywhere, Category = "Costs") int32 CostSkip = 1;
	UPROPERTY(EditAnywhere, Category = "Costs") int32 CostScout = 1;
	UPROPERTY(EditAnywhere, Category = "Costs") int32 CostLand = 2;

	// Skip behavior
	UPROPERTY(EditAnywhere, Category = "Skip") float SkipEncounterChance = 0.4f; // chance an encounter happens on skip
	UPROPERTY(EditAnywhere, Category = "Skip") float SkipBasePn = 0.65f;          // if skip encounter happens, base negative chance

	UPROPERTY(BlueprintAssignable, Category = "MiniGame")
	FOnMiniGameStarted OnMiniGameStarted;

	UPROPERTY(BlueprintAssignable, Category = "MiniGame")
	FOnMiniGameEnded OnMiniGameEnded;

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	bool IsWaitingMiniGame() const { return bWaitingMinigame; }

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	FMiniGameContext GetMiniGameContext() const { return CurrentMiniGame; }

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	bool CompleteMiniGame(EMiniGameResult Result, TArray<FText>& OutLogs);

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

	UPROPERTY() bool bWaitingMinigame = false;
	UPROPERTY() FMiniGameContext CurrentMiniGame;

	// Pending encounter that is waiting for mini-game resolution
	UPROPERTY() TObjectPtr<const UEncounterDefinition> PendingEncounter = nullptr;
	UPROPERTY() EPlanetSide PendingSide = EPlanetSide::A;
	UPROPERTY() TObjectPtr<UShipStateComponent> PendingShip = nullptr;

	FTimerHandle MiniGameTimeoutHandle;


	void StartMiniGameInternal(const UEncounterDefinition* Encounter, EPlanetSide Side, UShipStateComponent* Ship);
	void ResolveMiniGameInternal(EMiniGameResult Result, TArray<FText>& OutLogs);
	void HandleMiniGameTimeout();

	void FinalizeStepAndAdvance(UShipStateComponent* Ship, TArray<FText>& OutLogs);

	void LoadDatabase();
	void GenerateCurrentNode();

	// Resolution helpers
	float ComputeAdjustedPn(float BasePn, const UShipStateComponent* Ship, float ActionMultiplier) const;
	EOutcome RollOutcome(float PnAdjusted, FRandomStream& Stream) const;

	void ApplyOutcome(const UEncounterDefinition* Encounter, EOutcome Outcome, UShipStateComponent* Ship, TArray<FText>& OutLogs) const;
	void ApplyRules(const TArray<struct FConditionalEffectRule>& Rules, const FGameplayTagContainer& Modules, UShipStateComponent* Ship, TArray<FText>& OutLogs) const;

};
