#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LightFlickerComponent.generated.h"

class UPointLightComponent;
class AActor;

USTRUCT(BlueprintType)
struct FFlickerStageSettings
{
	GENERATED_BODY()

	// How fast the light moves toward its next random target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker")
	float InterpSpeed = 10.0f;

	// Time between target changes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker")
	float MinInterval = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker")
	float MaxInterval = 0.15f;

	// Normal random intensity range
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker")
	float NormalMinMultiplier = 0.85f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker")
	float NormalMaxMultiplier = 1.00f;

	// Chance of failure deep (big intensity drop)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker")
	float FailureChance = 0.08f;

	// Intensity range for failure deeps
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker")
	float FailureMinMultiplier = 0.10f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker")
	float FailureMaxMultiplier = 0.45f;
};

USTRUCT(BlueprintType)
struct FFlickerLightEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
	TObjectPtr<AActor> LightActor = nullptr;

	// Damage stage when this light starts flickering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (ClampMin = "1", ClampMax = "3"))
	int32 StartStage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
	bool bOverrideBaseIntensity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (EditCondition = "bOverrideBaseIntensity", ClampMin = "0.0"))
	float OverriddenBaseIntensity = 6500.0f;

	// Runtime only
	UPROPERTY(Transient)
	TObjectPtr<UPointLightComponent> ResolvedLight = nullptr;

	UPROPERTY(Transient)
	float BaseIntensity = 0.0f;

	UPROPERTY(Transient)
	float CurrentIntensity = 0.0f;

	UPROPERTY(Transient)
	float TargetIntensity = 0.0f;

	UPROPERTY(Transient)
	float TimeUntilNextTarget = 0.0f;
};

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class TERMINALSHUTDOWN_API ULightFlickerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULightFlickerComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	// List of lights to manage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker", meta = (TitleProperty = "LightActor"))
	TArray<FFlickerLightEntry> FlickerLights;

	// Settings for damage stage 1.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker|Stages")
	FFlickerStageSettings Stage1Settings;

	// Settings for damage stage 2.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker|Stages")
	FFlickerStageSettings Stage2Settings;

	// Settings for damage stage 3.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker|Stages")
	FFlickerStageSettings Stage3Settings;

	// Restore lights that should be inactive at this stage (for backwards if we repair ship)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker")
	bool bRestoreInactiveLightsToBase = true;

	// Apply flickering to lights based on stage.
	UFUNCTION(BlueprintCallable, Category = "Flicker")
	void ApplyDamageStage(int32 NewDamageStage);

	// Stops all flicker.
	UFUNCTION(BlueprintCallable, Category = "Flicker")
	void StopFlicker();

	UFUNCTION(BlueprintCallable, Category = "Flicker")
	void RefreshBaseIntensities();

	// Debugging.
	UFUNCTION(BlueprintPure, Category = "Flicker")
	int32 GetCurrentDamageStage() const { return CurrentDamageStage; }

private:
	UPROPERTY(Transient)
	int32 CurrentDamageStage = 0;

private:
	void ResolveLights();
	void InitializeRuntimeState(bool bForceBaseRefresh);
	void UpdateLight(FFlickerLightEntry& Entry, float DeltaTime, const FFlickerStageSettings& Settings);
	void ChooseNewTarget(FFlickerLightEntry& Entry, const FFlickerStageSettings& Settings);
	void RestoreLightToBase(FFlickerLightEntry& Entry);
	const FFlickerStageSettings& GetSettingsForStage(int32 DamageStage) const;
	UPointLightComponent* ResolvePointLightComponent(AActor* Actor) const;
	bool HasAnyActiveLightsForCurrentStage() const;
};