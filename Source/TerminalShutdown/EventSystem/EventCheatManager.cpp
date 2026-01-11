#include "EventSystem/EventCheatManager.h"

#include "EventSystem/EventRunSubsystem.h"
#include "EventSystem/ShipStateComponent.h"
#include "GameFramework/PlayerController.h"

static UShipStateComponent* GetShipState(APlayerController* PC)
{
	if (!PC) return nullptr;
	APawn* P = PC->GetPawn();
	return P ? P->FindComponentByClass<UShipStateComponent>() : nullptr;
}

static bool CheckRunActive(UEventRunSubsystem* Sys)
{
	if (!Sys) return false;
	if (Sys->IsRunEnded())
	{
		UE_LOG(LogTemp, Warning, TEXT("=== RUN ENDED ==="));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Sys->GetEndReason().ToString());
		return false;
	}
	return true;
}

void UEventCheatManager::EV_Start(int32 Seed)
{
	APlayerController* PC = GetOuterAPlayerController();
	if (!PC) return;

	UEventRunSubsystem* Sys = PC->GetGameInstance()->GetSubsystem<UEventRunSubsystem>();
	if (!Sys) return;

	Sys->StartRun(Seed);
	EV_Show();
}

void UEventCheatManager::EV_Show()
{
	APlayerController* PC = GetOuterAPlayerController();
	UShipStateComponent* Ship = GetShipState(PC);
	if (!PC || !Ship) return;

	UEventRunSubsystem* Sys = PC->GetGameInstance()->GetSubsystem<UEventRunSubsystem>();
	if (!Sys) return;

	UE_LOG(LogTemp, Warning, TEXT("=== CURRENT NODE VIEW ==="));
	const FRunNodeView View = Sys->GetCurrentNodeView(Ship);
	UE_LOG(LogTemp, Warning, TEXT("Ship: Energy=%d Damage=%d"), Ship->Energy, Ship->Damage);

	UE_LOG(LogTemp, Warning, TEXT("A: %s"), *View.PlanetA.Preview.ToString());
	UE_LOG(LogTemp, Warning, TEXT("A: %s"), *View.PlanetA.GeneralHint.ToString());
	if (View.PlanetA.bScouted) UE_LOG(LogTemp, Warning, TEXT("   Scout: %s"), *View.PlanetA.ScoutInfo.ToString());

	UE_LOG(LogTemp, Warning, TEXT("Skip: cost=%d"), View.Skip.EnergyCost);
	UE_LOG(LogTemp, Warning, TEXT("B: %s"), *View.PlanetB.Preview.ToString());
	UE_LOG(LogTemp, Warning, TEXT("B: %s"), *View.PlanetB.GeneralHint.ToString());
	if (View.PlanetB.bScouted) UE_LOG(LogTemp, Warning, TEXT("   Scout: %s"), *View.PlanetB.ScoutInfo.ToString());
}

static void DoChoice(APlayerController* PC, ERunChoice Choice)
{
	UShipStateComponent* Ship = GetShipState(PC);
	if (!PC || !Ship) return;

	UEventRunSubsystem* Sys = PC->GetGameInstance()->GetSubsystem<UEventRunSubsystem>();
	if (!Sys) return;
	if (!CheckRunActive(Sys)) return;

	const int32 EnergyBefore = Ship->Energy;
	const int32 DamageBefore = Ship->Damage;

	TArray<FText> Logs;
	const bool bOk = Sys->Choose(Ship, Choice, Logs);

	if (!bOk)
	{
		UE_LOG(LogTemp, Warning, TEXT("Choice failed (not enough energy, missing DB, invalid planet, etc.)"));
		return;
	}

	const int32 EnergyAfter = Ship->Energy;
	const int32 DamageAfter = Ship->Damage;

	const int32 DeltaE = EnergyAfter - EnergyBefore;
	const int32 DeltaD = DamageAfter - DamageBefore;

	const TCHAR* ChoiceName =
		(Choice == ERunChoice::PlanetA) ? TEXT("Planet A") :
		(Choice == ERunChoice::PlanetB) ? TEXT("Planet B") :
		TEXT("Skip");

	UE_LOG(LogTemp, Warning, TEXT("=== RESULT (%s) ==="), ChoiceName);

	for (const FText& L : Logs)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *L.ToString());
	}

	UE_LOG(LogTemp, Warning, TEXT("Energy: %d -> %d  (Δ %s%d)"),
		EnergyBefore, EnergyAfter, (DeltaE >= 0 ? TEXT("+") : TEXT("")), DeltaE);
	UE_LOG(LogTemp, Warning, TEXT("Damage: %d -> %d  (Δ %s%d)"),
		DamageBefore, DamageAfter, (DeltaD >= 0 ? TEXT("+") : TEXT("")), DeltaD);
	if (Sys->IsRunEnded())
	{
		UE_LOG(LogTemp, Warning, TEXT("=== RUN ENDED ==="));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Sys->GetEndReason().ToString());
	}


}

void UEventCheatManager::EV_ScoutA()
{
	APlayerController* PC = GetOuterAPlayerController();
	UShipStateComponent* Ship = GetShipState(PC);
	if (!PC || !Ship) return;

	UEventRunSubsystem* Sys = PC->GetGameInstance()->GetSubsystem<UEventRunSubsystem>();
	if (!Sys) return;

	TArray<FText> Logs;
	Sys->Scout(Ship, EPlanetSide::A, Logs);
	for (const FText& L : Logs)
		UE_LOG(LogTemp, Warning, TEXT("%s"), *L.ToString());
}

void UEventCheatManager::EV_ScoutB()
{
	APlayerController* PC = GetOuterAPlayerController();
	UShipStateComponent* Ship = GetShipState(PC);
	if (!PC || !Ship) return;

	UEventRunSubsystem* Sys = PC->GetGameInstance()->GetSubsystem<UEventRunSubsystem>();
	if (!Sys) return;

	TArray<FText> Logs;
	Sys->Scout(Ship, EPlanetSide::B, Logs);
	for (const FText& L : Logs)
		UE_LOG(LogTemp, Warning, TEXT("%s"), *L.ToString());
}

static void SetModuleTag(APlayerController* PC, const TCHAR* TagName, bool bEnable)
{
	UShipStateComponent* Ship = GetShipState(PC);
	if (!PC || !Ship)
	{
		UE_LOG(LogTemp, Warning, TEXT("Module toggle failed: missing PlayerController or ShipStateComponent."));
		return;
	}

	const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TagName), false);
	if (!Tag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameplayTag not found: %s"), TagName);
		return;
	}

	if (bEnable)
	{
		Ship->ActiveModules.AddTag(Tag);
		UE_LOG(LogTemp, Warning, TEXT("%s ENABLED"), TagName);
	}
	else
	{
		Ship->ActiveModules.RemoveTag(Tag);
		UE_LOG(LogTemp, Warning, TEXT("%s DISABLED"), TagName);
	}

	FString TagsStr;
	for (const FGameplayTag& T : Ship->ActiveModules)
	{
		if (!TagsStr.IsEmpty()) TagsStr += TEXT(", ");
		TagsStr += T.ToString();
	}
	UE_LOG(LogTemp, Warning, TEXT("ActiveModules: [%s]"), *TagsStr);
}

void UEventCheatManager::EV_TurretOn()
{
	SetModuleTag(GetOuterAPlayerController(), TEXT("Module.Turret"), true);
}

void UEventCheatManager::EV_TurretOff()
{
	SetModuleTag(GetOuterAPlayerController(), TEXT("Module.Turret"), false);
}

void UEventCheatManager::EV_ShieldOn()
{
	SetModuleTag(GetOuterAPlayerController(), TEXT("Module.Shield"), true);
}

void UEventCheatManager::EV_ShieldOff()
{
	SetModuleTag(GetOuterAPlayerController(), TEXT("Module.Shield"), false);
}

void UEventCheatManager::EV_ScannerOn()
{
	SetModuleTag(GetOuterAPlayerController(), TEXT("Module.Scanner"), true);
}

void UEventCheatManager::EV_ScannerOff()
{
	SetModuleTag(GetOuterAPlayerController(), TEXT("Module.Scanner"), false);
}



void UEventCheatManager::EV_ChooseA() { DoChoice(GetOuterAPlayerController(), ERunChoice::PlanetA); EV_Show(); }
void UEventCheatManager::EV_ChooseB() { DoChoice(GetOuterAPlayerController(), ERunChoice::PlanetB); EV_Show(); }
void UEventCheatManager::EV_Skip() { DoChoice(GetOuterAPlayerController(), ERunChoice::Skip);    EV_Show(); }
