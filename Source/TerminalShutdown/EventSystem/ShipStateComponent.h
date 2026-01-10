#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ShipStateComponent.generated.h"

UCLASS(ClassGroup = (Game), meta = (BlueprintSpawnableComponent))
class TERMINALSHUTDOWN_API UShipStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	int32 Damage = 0; // 0..3 (3 is game over)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	int32 Energy = 10; // start at 10, max 20

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	FGameplayTagContainer ActiveModules; // e.g. Module.Turret, Module.Shield

	UFUNCTION(BlueprintCallable, Category = "Ship")
	bool IsGameOver() const { return Damage >= 3 || Energy <= 0; }
};
