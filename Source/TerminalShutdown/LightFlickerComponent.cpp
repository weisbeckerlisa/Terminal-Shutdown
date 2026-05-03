#include "LightFlickerComponent.h"

#include "Components/PointLightComponent.h"
#include "Engine/PointLight.h"
#include "GameFramework/Actor.h"

ULightFlickerComponent::ULightFlickerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false);

	Stage1Settings.InterpSpeed = 10.0f;
	Stage1Settings.MinInterval = 0.06f;
	Stage1Settings.MaxInterval = 0.16f;
	Stage1Settings.NormalMinMultiplier = 0.85f;
	Stage1Settings.NormalMaxMultiplier = 1.00f;
	Stage1Settings.FailureChance = 0.06f;
	Stage1Settings.FailureMinMultiplier = 0.20f;
	Stage1Settings.FailureMaxMultiplier = 0.50f;

	Stage2Settings.InterpSpeed = 12.0f;
	Stage2Settings.MinInterval = 0.04f;
	Stage2Settings.MaxInterval = 0.12f;
	Stage2Settings.NormalMinMultiplier = 0.65f;
	Stage2Settings.NormalMaxMultiplier = 1.00f;
	Stage2Settings.FailureChance = 0.12f;
	Stage2Settings.FailureMinMultiplier = 0.05f;
	Stage2Settings.FailureMaxMultiplier = 0.35f;

	Stage3Settings.InterpSpeed = 15.0f;
	Stage3Settings.MinInterval = 0.03f;
	Stage3Settings.MaxInterval = 0.08f;
	Stage3Settings.NormalMinMultiplier = 0.35f;
	Stage3Settings.NormalMaxMultiplier = 1.00f;
	Stage3Settings.FailureChance = 0.22f;
	Stage3Settings.FailureMinMultiplier = 0.00f;
	Stage3Settings.FailureMaxMultiplier = 0.20f;
}

void ULightFlickerComponent::BeginPlay()
{
	Super::BeginPlay();

	ResolveLights();
	InitializeRuntimeState(true);
	StopFlicker();
}

void ULightFlickerComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentDamageStage <= 0)
	{
		return;
	}

	const FFlickerStageSettings& Settings = GetSettingsForStage(CurrentDamageStage);

	bool bAnyActive = false;

	for (FFlickerLightEntry& Entry : FlickerLights)
	{
		if (!IsValid(Entry.ResolvedLight))
		{
			continue;
		}

		if (CurrentDamageStage >= Entry.StartStage)
		{
			bAnyActive = true;
			UpdateLight(Entry, DeltaTime, Settings);
		}
		else if (bRestoreInactiveLightsToBase)
		{
			RestoreLightToBase(Entry);
		}
	}

	if (!bAnyActive)
	{
		SetComponentTickEnabled(false);
	}
}

void ULightFlickerComponent::ApplyDamageStage(int32 NewDamageStage)
{
	CurrentDamageStage = FMath::Clamp(NewDamageStage, 0, 3);

	if (CurrentDamageStage <= 0)
	{
		StopFlicker();
		return;
	}

	// Ensure references are still valid
	ResolveLights();

	const FFlickerStageSettings& Settings = GetSettingsForStage(CurrentDamageStage);

	bool bAnyActive = false;

	for (FFlickerLightEntry& Entry : FlickerLights)
	{
		if (!IsValid(Entry.ResolvedLight))
		{
			continue;
		}

		if (CurrentDamageStage >= Entry.StartStage)
		{
			bAnyActive = true;

			// Snap runtime state to current output if this light was not active before
			if (Entry.CurrentIntensity <= 0.0f)
			{
				Entry.CurrentIntensity = Entry.BaseIntensity;
			}

			ChooseNewTarget(Entry, Settings);
		}
		else if (bRestoreInactiveLightsToBase)
		{
			RestoreLightToBase(Entry);
		}
	}

	SetComponentTickEnabled(bAnyActive);
}

void ULightFlickerComponent::StopFlicker()
{
	CurrentDamageStage = 0;

	for (FFlickerLightEntry& Entry : FlickerLights)
	{
		RestoreLightToBase(Entry);
	}

	SetComponentTickEnabled(false);
}

void ULightFlickerComponent::RefreshBaseIntensities()
{
	ResolveLights();

	for (FFlickerLightEntry& Entry : FlickerLights)
	{
		if (!IsValid(Entry.ResolvedLight))
		{
			continue;
		}

		if (Entry.bOverrideBaseIntensity)
		{
			Entry.BaseIntensity = Entry.OverriddenBaseIntensity;
		}
		else
		{
			Entry.BaseIntensity = Entry.ResolvedLight->Intensity;
		}

		Entry.CurrentIntensity = Entry.BaseIntensity;
		Entry.TargetIntensity = Entry.BaseIntensity;
		Entry.TimeUntilNextTarget = 0.0f;
	}
}

void ULightFlickerComponent::ResolveLights()
{
	for (FFlickerLightEntry& Entry : FlickerLights)
	{
		Entry.ResolvedLight = ResolvePointLightComponent(Entry.LightActor);
	}
}

void ULightFlickerComponent::InitializeRuntimeState(bool bForceBaseRefresh)
{
	for (FFlickerLightEntry& Entry : FlickerLights)
	{
		if (!IsValid(Entry.ResolvedLight))
		{
			continue;
		}

		if (bForceBaseRefresh)
		{
			if (Entry.bOverrideBaseIntensity)
			{
				Entry.BaseIntensity = Entry.OverriddenBaseIntensity;
			}
			else
			{
				Entry.BaseIntensity = Entry.ResolvedLight->Intensity;
			}
		}

		Entry.CurrentIntensity = Entry.BaseIntensity;
		Entry.TargetIntensity = Entry.BaseIntensity;
		Entry.TimeUntilNextTarget = FMath::FRandRange(0.02f, 0.12f);

		Entry.ResolvedLight->SetIntensity(Entry.BaseIntensity);
	}
}

void ULightFlickerComponent::UpdateLight(
	FFlickerLightEntry& Entry,
	float DeltaTime,
	const FFlickerStageSettings& Settings)
{
	if (!IsValid(Entry.ResolvedLight))
	{
		return;
	}

	Entry.TimeUntilNextTarget -= DeltaTime;

	if (Entry.TimeUntilNextTarget <= 0.0f)
	{
		ChooseNewTarget(Entry, Settings);
	}

	Entry.CurrentIntensity = FMath::FInterpTo(
		Entry.CurrentIntensity,
		Entry.TargetIntensity,
		DeltaTime,
		Settings.InterpSpeed
	);

	Entry.ResolvedLight->SetIntensity(Entry.CurrentIntensity);
}

void ULightFlickerComponent::ChooseNewTarget(
	FFlickerLightEntry& Entry,
	const FFlickerStageSettings& Settings)
{
	if (!IsValid(Entry.ResolvedLight))
	{
		return;
	}

	const bool bUseFailureDip = (FMath::FRand() < Settings.FailureChance);

	float Multiplier = 1.0f;

	if (bUseFailureDip)
	{
		Multiplier = FMath::FRandRange(
			Settings.FailureMinMultiplier,
			Settings.FailureMaxMultiplier
		);
	}
	else
	{
		Multiplier = FMath::FRandRange(
			Settings.NormalMinMultiplier,
			Settings.NormalMaxMultiplier
		);
	}

	Entry.TargetIntensity = Entry.BaseIntensity * Multiplier;

	Entry.TimeUntilNextTarget = FMath::FRandRange(Settings.MinInterval, Settings.MaxInterval);
}

void ULightFlickerComponent::RestoreLightToBase(FFlickerLightEntry& Entry)
{
	if (!IsValid(Entry.ResolvedLight))
	{
		return;
	}

	Entry.CurrentIntensity = Entry.BaseIntensity;
	Entry.TargetIntensity = Entry.BaseIntensity;
	Entry.TimeUntilNextTarget = 0.0f;

	Entry.ResolvedLight->SetIntensity(Entry.BaseIntensity);
}

const FFlickerStageSettings& ULightFlickerComponent::GetSettingsForStage(int32 DamageStage) const
{
	if (DamageStage <= 1)
	{
		return Stage1Settings;
	}
	if (DamageStage == 2)
	{
		return Stage2Settings;
	}
	return Stage3Settings;
}

UPointLightComponent* ULightFlickerComponent::ResolvePointLightComponent(AActor* Actor) const
{
	if (!IsValid(Actor))
	{
		return nullptr;
	}

	if (APointLight* PointLightActor = Cast<APointLight>(Actor))
	{
		return PointLightActor->PointLightComponent;
	}

	return Actor->FindComponentByClass<UPointLightComponent>();
}

bool ULightFlickerComponent::HasAnyActiveLightsForCurrentStage() const
{
	if (CurrentDamageStage <= 0)
	{
		return false;
	}

	for (const FFlickerLightEntry& Entry : FlickerLights)
	{
		if (IsValid(Entry.ResolvedLight) && CurrentDamageStage >= Entry.StartStage)
		{
			return true;
		}
	}

	return false;
}