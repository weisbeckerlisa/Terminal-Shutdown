#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EventSystemSettings.generated.h"

class UEventDatabase;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Event System"))
class TERMINALSHUTDOWN_API UEventSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "Database")
	TSoftObjectPtr<UEventDatabase> EventDatabase;
};
