#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ShipStateComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShipStateChanged);


UCLASS(ClassGroup = (Game), meta = (BlueprintSpawnableComponent))
class TERMINALSHUTDOWN_API UShipStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	int32 Damage = 0; // 0..3 (3 is game over)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	int32 Energy = 10; // start at 10, max 20, <0 is game over

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	FGameplayTagContainer ActiveModules; // e.g. Module.Turret, Module.Shield, Module.Scanner

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	int32 StepsCompleted = 0;

	UFUNCTION(BlueprintCallable, Category = "Ship")
	bool IsGameOver() const { return Damage >= 3 || Energy < 0; }

	UFUNCTION(BlueprintCallable, Category = "Ship")
	bool IsGameWon() const { return StepsCompleted >= 10; }

	// Fired whenever energy/damage/modules/etc change
	UPROPERTY(BlueprintAssignable, Category = "Ship")
	FOnShipStateChanged OnShipStateChanged;

	// Toggle/force a module state and notify listeners
	UFUNCTION(BlueprintCallable, Category = "Ship|Modules")
	void SetModuleEnabled(FGameplayTag ModuleTag, bool bEnabled);

	// Helper
	UFUNCTION(BlueprintCallable, Category = "Ship|Modules")
	bool IsModuleEnabled(FGameplayTag ModuleTag) const;


};
