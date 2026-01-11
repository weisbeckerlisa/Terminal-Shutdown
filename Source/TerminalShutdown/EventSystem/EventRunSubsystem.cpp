#include "EventSystem/EventRunSubsystem.h"

#include "EventSystem/EventSystemSettings.h"
#include "EventSystem/EventDatabase.h"
#include "EventSystem/PlanetDefinition.h"
#include "EventSystem/EncounterDefinition.h"
#include "EventSystem/ShipStateComponent.h"

#include "Engine/AssetManager.h"
#include "Kismet/KismetMathLibrary.h"

static float Sigmoid(float X)
{
	return 1.0f / (1.0f + FMath::Exp(-X));
}

void UEventRunSubsystem::LoadDatabase()
{
	if (DB) return;

	const UEventSystemSettings* Settings = GetDefault<UEventSystemSettings>();
	if (!Settings || Settings->EventDatabase.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("[EventRunSubsystem] EventDatabase is not set. Set it in Project Settings -> Event System."));
		return;
	}

	DB = Settings->EventDatabase.LoadSynchronous();
	if (!DB)
	{
		UE_LOG(LogTemp, Error, TEXT("[EventRunSubsystem] Failed to load EventDatabase asset."));
	}
}

void UEventRunSubsystem::StartRun(int32 Seed)
{
	LoadDatabase();
	if (!DB) return;

	Rng.Initialize(Seed);
	Current = FRunNode{};
	Current.StepIndex = 0;
	bRunEnded = false;
	EndReason = FText::GetEmpty();
	GenerateCurrentNode();
}

void UEventRunSubsystem::GenerateCurrentNode()
{
	if (!DB) return;

	// Pick Planet A/B randomly from DB->Planets
	if (DB->Planets.Num() < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("[EventRunSubsystem] Need at least 2 planets in DB."));
		return;
	}

	int32 IndexA = Rng.RandRange(0, DB->Planets.Num() - 1);
	int32 IndexB = Rng.RandRange(0, DB->Planets.Num() - 1);
	if (IndexB == IndexA)
	{
		IndexB = (IndexA + 1) % DB->Planets.Num();
	}

	Current.PlanetA = DB->Planets[IndexA];
	Current.PlanetB = DB->Planets[IndexB];
	Current.LockedA = EOutcome::Unknown;
	Current.LockedB = EOutcome::Unknown;
	Current.SkipEncounter = nullptr;
}

FRunNodeView UEventRunSubsystem::GetCurrentNodeView(UShipStateComponent* Ship) const
{
	if (bRunEnded)
	{
		FRunNodeView View;
		View.PlanetA.Label = FText::FromString(TEXT("RUN ENDED"));
		View.PlanetA.Preview = EndReason;
		return View;
	}
	FRunNodeView View;

	// Planet A
	View.PlanetA.Label = FText::FromString(TEXT("Planet A"));
	View.PlanetA.Preview = Current.PlanetA ? Current.PlanetA->PlanetPreviewDescription : FText::FromString(TEXT("No planet"));
	View.PlanetA.GeneralHint = Current.PlanetA && Current.PlanetA->Encounter ? Current.PlanetA->Encounter->GeneralHint : FText::FromString(TEXT("No encounter"));
	View.PlanetA.EnergyCost = CostLand;
	View.PlanetA.bCanScout = true;
	View.PlanetA.bScouted = (Current.LockedA != EOutcome::Unknown);

	// Skip
	View.Skip.Label = FText::FromString(TEXT("Skip"));
	View.Skip.Preview = FText::FromString(TEXT("Continue travelling."));
	View.Skip.EnergyCost = CostSkip;
	View.Skip.bCanScout = false;
	View.Skip.bScouted = false;

	// Planet B
	View.PlanetB.Label = FText::FromString(TEXT("Planet B"));
	View.PlanetB.Preview = Current.PlanetB ? Current.PlanetB->PlanetPreviewDescription : FText::FromString(TEXT("No planet"));
	View.PlanetB.GeneralHint = Current.PlanetB && Current.PlanetB->Encounter ? Current.PlanetB->Encounter->GeneralHint : FText::FromString(TEXT("No encounter"));
	View.PlanetB.EnergyCost = CostLand;
	View.PlanetB.bCanScout = true;
	View.PlanetB.bScouted = (Current.LockedB != EOutcome::Unknown);

	// If already scouted, show the precise scout description
	if (Current.PlanetA && Current.PlanetA->Encounter && Current.LockedA != EOutcome::Unknown)
	{
		const auto& OutcomeDef = (Current.LockedA == EOutcome::Negative) ? Current.PlanetA->Encounter->Negative : Current.PlanetA->Encounter->Positive;
		View.PlanetA.ScoutInfo = OutcomeDef.ScoutDescription;
	}
	if (Current.PlanetB && Current.PlanetB->Encounter && Current.LockedB != EOutcome::Unknown)
	{
		const auto& OutcomeDef = (Current.LockedB == EOutcome::Negative) ? Current.PlanetB->Encounter->Negative : Current.PlanetB->Encounter->Positive;
		View.PlanetB.ScoutInfo = OutcomeDef.ScoutDescription;
	}

	return View;
}

float UEventRunSubsystem::ComputeAdjustedPn(float BasePn, const UShipStateComponent* Ship, float ActionMultiplier) const
{
	// Condition score S in [0,1]
	const float XDmg = FMath::Clamp((float)Ship->Damage / 2.0f, 0.f, 1.f);
	const float XEnergy = FMath::Clamp(1.0f - ((float)Ship->Energy / (float)MaxEnergy), 0.f, 1.f);
	const float S = WDmg * XDmg + WEnergy * XEnergy;

	// logit(BasePn)
	const float P = FMath::Clamp(BasePn, 0.001f, 0.999f);
	const float L0 = FMath::Loge(P / (1.0f - P));

	// shift
	const float Delta = ActionMultiplier * K * (STarget - S);

	// back to probability
	return Sigmoid(L0 + Delta);
}

EOutcome UEventRunSubsystem::RollOutcome(float PnAdjusted, FRandomStream& Stream) const
{
	const float Roll = Stream.FRand();
	return (Roll < PnAdjusted) ? EOutcome::Negative : EOutcome::Positive;
}



bool UEventRunSubsystem::Scout(UShipStateComponent* Ship, EPlanetSide Side, TArray<FText>& OutLogs)
{
	if (bRunEnded)
	{
		OutLogs.Add(FText::FromString(TEXT("Run already ended.")));
		return false;
	}
	LoadDatabase();
	if (!DB || !Ship) return false;
	if (Ship->Energy < CostScout) return false;

	const UPlanetDefinition* Planet = (Side == EPlanetSide::A) ? Current.PlanetA : Current.PlanetB;
	if (!Planet || !Planet->Encounter) return false;

	EOutcome& Locked = (Side == EPlanetSide::A) ? Current.LockedA : Current.LockedB;
	if (Locked != EOutcome::Unknown) return true; // already scouted

	Ship->Energy -= CostScout;

	const float PnAdj = ComputeAdjustedPn(Planet->BasePn, Ship, /*ActionMultiplier*/0.7f);
	Locked = RollOutcome(PnAdj, Rng);

	const FOutcomeDefinition& Def = (Locked == EOutcome::Negative) ? Planet->Encounter->Negative : Planet->Encounter->Positive;
	OutLogs.Add(FText::FromString(TEXT("[SCOUT] ")));
	OutLogs.Add(Def.ScoutDescription);

	return true;
}

void UEventRunSubsystem::ApplyRules(const TArray<FConditionalEffectRule>& Rules, const FGameplayTagContainer& Modules, UShipStateComponent* Ship, TArray<FText>& OutLogs) const
{
	// First-match semantics: apply first rule whose condition matches (or empty = always)
	for (const FConditionalEffectRule& Rule : Rules)
	{
		const bool bConditionEmpty = Rule.Condition.IsEmpty();
		const bool bMatches = bConditionEmpty || Rule.Condition.Matches(Modules);

		if (!bMatches) continue;

		Ship->Energy = FMath::Clamp(Ship->Energy + Rule.Effect.EnergyDelta, -5, MaxEnergy);
		Ship->Damage = FMath::Clamp(Ship->Damage + Rule.Effect.DamageDelta, 0, 3);

		for (const FText& L : Rule.ExtraLogs)
			OutLogs.Add(L);

		Ship->ActiveModules.AppendTags(Rule.Effect.AddTags);
		for (const FGameplayTag& T : Rule.Effect.RemoveTags)
			Ship->ActiveModules.RemoveTag(T);

		break;
	}
}

void UEventRunSubsystem::ApplyOutcome(const UEncounterDefinition* Encounter, EOutcome Outcome, UShipStateComponent* Ship, TArray<FText>& OutLogs) const
{
	if (!Encounter || !Ship) return;

	const FOutcomeDefinition& Def = (Outcome == EOutcome::Negative) ? Encounter->Negative : Encounter->Positive;

	for (const FText& L : Def.ResolveLogs)
		OutLogs.Add(L);

	ApplyRules(Def.ConditionalRules, Ship->ActiveModules, Ship, OutLogs);
}

bool UEventRunSubsystem::Choose(UShipStateComponent* Ship, ERunChoice Choice, TArray<FText>& OutLogs)
{
	if (bRunEnded)
	{
		OutLogs.Add(FText::FromString(TEXT("Run already ended.")));
		return false;
	}

	LoadDatabase();
	if (!DB || !Ship) return false;

	// energy costs
	const int32 BaseCost = (Choice == ERunChoice::Skip) ? CostSkip : CostLand;
	const int32 ModuleCost = Ship->ActiveModules.Num();
	const int32 Cost = BaseCost + ModuleCost;
	if (Ship->Energy < Cost) return false;
	Ship->Energy -= Cost;

	if (Choice == ERunChoice::Skip)
	{
		// Skip: sometimes trigger a travel encounter (usually negative)
		if (DB->TravelEncounters.Num() > 0 && Rng.FRand() < SkipEncounterChance)
		{
			const int32 Idx = Rng.RandRange(0, DB->TravelEncounters.Num() - 1);
			Current.SkipEncounter = DB->TravelEncounters[Idx];

			const float PnAdj = ComputeAdjustedPn(SkipBasePn, Ship, 0.3f); // ActionMultiplier=0.3 for skip
			const EOutcome Outcome = RollOutcome(PnAdj, Rng);

			OutLogs.Add(FText::FromString(TEXT("[SKIP EVENT]")));
			ApplyOutcome(Current.SkipEncounter, Outcome, Ship, OutLogs);
		}
		else
		{
			OutLogs.Add(FText::FromString(TEXT("You drift forward. Nothing happens.")));
		}
	}
	else
	{
		const bool bA = (Choice == ERunChoice::PlanetA);
		const UPlanetDefinition* Planet = bA ? Current.PlanetA : Current.PlanetB;
		if (!Planet || !Planet->Encounter) return false;

		EOutcome Locked = bA ? Current.LockedA : Current.LockedB;
		if (Locked == EOutcome::Unknown)
		{
			const float PnAdj = ComputeAdjustedPn(Planet->BasePn, Ship, /*ActionMultiplier*/1.0f);
			Locked = RollOutcome(PnAdj, Rng);
		}

		OutLogs.Add(FText::FromString(TEXT("[LANDING]")));
		ApplyOutcome(Planet->Encounter, Locked, Ship, OutLogs);
	}

	Ship->StepsCompleted++;
	if (Ship->IsGameWon())
	{
		bRunEnded = true;
		EndReason = FText::FromString(TEXT("MISSION COMPLETE: You reached Earth !"));
		OutLogs.Add(EndReason);
		return true;
	}

	if (Ship->Damage >= 3)
	{
		bRunEnded = true;
		EndReason = FText::FromString(TEXT("GAME OVER: The ship is too damaged to continue."));
		OutLogs.Add(EndReason);
		return true;
	}

	if (Ship->Energy <= 0)
	{
		bRunEnded = true;
		EndReason = FText::FromString(TEXT("GAME OVER: You ran out of energy. All systems Shutdown."));
		OutLogs.Add(EndReason);
		return true;
	}

	Current.StepIndex++;
	GenerateCurrentNode();
	return true;
}
