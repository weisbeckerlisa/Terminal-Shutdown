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

	const FRunNodeView View = Sys->GetCurrentNodeView(Ship);
	UE_LOG(LogTemp, Warning, TEXT("Ship: Energy=%d Damage=%d"), Ship->Energy, Ship->Damage);

	UE_LOG(LogTemp, Warning, TEXT("A: %s"), *View.PlanetA.Preview.ToString());
	if (View.PlanetA.bScouted) UE_LOG(LogTemp, Warning, TEXT("   Scout: %s"), *View.PlanetA.ScoutInfo.ToString());

	UE_LOG(LogTemp, Warning, TEXT("Skip: cost=%d"), View.Skip.EnergyCost);

	UE_LOG(LogTemp, Warning, TEXT("B: %s"), *View.PlanetB.Preview.ToString());
	if (View.PlanetB.bScouted) UE_LOG(LogTemp, Warning, TEXT("   Scout: %s"), *View.PlanetB.ScoutInfo.ToString());
}

static void DoChoice(APlayerController* PC, ERunChoice Choice)
{
	UShipStateComponent* Ship = GetShipState(PC);
	if (!PC || !Ship) return;

	UEventRunSubsystem* Sys = PC->GetGameInstance()->GetSubsystem<UEventRunSubsystem>();
	if (!Sys) return;

	TArray<FText> Logs;
	Sys->Choose(Ship, Choice, Logs);
	for (const FText& L : Logs)
		UE_LOG(LogTemp, Warning, TEXT("%s"), *L.ToString());
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

void UEventCheatManager::EV_ChooseA() { DoChoice(GetOuterAPlayerController(), ERunChoice::PlanetA); EV_Show(); }
void UEventCheatManager::EV_ChooseB() { DoChoice(GetOuterAPlayerController(), ERunChoice::PlanetB); EV_Show(); }
void UEventCheatManager::EV_Skip() { DoChoice(GetOuterAPlayerController(), ERunChoice::Skip);    EV_Show(); }
