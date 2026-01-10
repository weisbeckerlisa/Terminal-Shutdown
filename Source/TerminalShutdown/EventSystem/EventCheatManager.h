#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "EventRunTypes.h"
#include "EventCheatManager.generated.h"

UCLASS()
class TERMINALSHUTDOWN_API UEventCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec) void EV_Start(int32 Seed = 12345);
	UFUNCTION(Exec) void EV_Show();
	UFUNCTION(Exec) void EV_ScoutA();
	UFUNCTION(Exec) void EV_ScoutB();
	UFUNCTION(Exec) void EV_ChooseA();
	UFUNCTION(Exec) void EV_ChooseB();
	UFUNCTION(Exec) void EV_Skip();
};
