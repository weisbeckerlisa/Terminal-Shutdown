#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ShipStateComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShipStateChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShipModulesChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVitalsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuppliesChanged);


UCLASS(ClassGroup = (Game), meta = (BlueprintSpawnableComponent))
class TERMINALSHUTDOWN_API UShipStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	int32 Damage = 0; // 0..4 (4 is game over)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	int32 Energy = 15; // start at 15, max 30, <0 is game over

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	FGameplayTagContainer ActiveModules; // e.g. Module.Turret, Module.Shield, Module.Scanner

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	int32 StepsCompleted = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Balancing")
	int32 StepsToEarth = 15;

	UFUNCTION(BlueprintCallable, Category = "Ship")
	bool IsGameOver() const { return Damage >= 4 || Energy == 0 || Hunger < 0 || Thirst < 0; }

	UFUNCTION(BlueprintCallable, Category = "Ship")
	bool IsGameWon() const { return StepsCompleted >= StepsToEarth; }

	// Fired whenever energy/damage/modules/etc change
	UPROPERTY(BlueprintAssignable, Category = "Ship")
	FOnShipStateChanged OnShipStateChanged;

	// Fired whenever modules change
	UPROPERTY(BlueprintAssignable, Category = "Ship")
	FOnShipModulesChanged OnShipModulesChanged;


	// Toggle/force a module state and notify listeners
	UFUNCTION(BlueprintCallable, Category = "Ship|Modules")
	void SetModuleEnabled(FGameplayTag ModuleTag, bool bEnabled);

	// Helper
	UFUNCTION(BlueprintCallable, Category = "Ship|Modules")
	bool IsModuleEnabled(FGameplayTag ModuleTag) const;

	// Player vitals and supplies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Vitals")
	int32 FoodUnits = 3; // max 16, min 0

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Vitals")
	int32 WaterUnits = 3; // max 16, min 0

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Vitals")
	float Hunger = 75.f; // 0..100

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Vitals")
	float Thirst = 75.f; // 0..100

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Vitals")
	float HungerPerDay = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Vitals")
	float ThirstPerDay = 20.f;
	
	UFUNCTION(BlueprintCallable, Category = "Ship|Vitals")
	void AdvanceDay(int32 days = 1);

	UFUNCTION(BlueprintCallable, Category = "Ship|Vitals")
	bool ConsumeFood(float RestoreAmount = 30.f);

	UFUNCTION(BlueprintCallable, Category = "Ship|Vitals")
	bool ConsumeWater(float RestoreAmount = 50.f);

	UPROPERTY(BlueprintAssignable, Category = "Ship|Vitals")
	FOnVitalsChanged OnVitalsChanged; 
	
	UPROPERTY(BlueprintAssignable, Category = "Ship|Vitals") 
	FOnSuppliesChanged OnSuppliesChanged;

};
