#include "EventSystem/ShipStateComponent.h"

void UShipStateComponent::SetModuleEnabled(FGameplayTag ModuleTag, bool bEnabled)
{
	if (!ModuleTag.IsValid())
	{
		return;
	}

	const bool bAlreadyEnabled = ActiveModules.HasTagExact(ModuleTag);

	if (bEnabled)
	{
		if (!bAlreadyEnabled)
		{
			ActiveModules.AddTag(ModuleTag);
			OnShipStateChanged.Broadcast();
		}
	}
	else
	{
		if (bAlreadyEnabled)
		{
			ActiveModules.RemoveTag(ModuleTag);
			OnShipStateChanged.Broadcast();
		}
	}
}

bool UShipStateComponent::IsModuleEnabled(FGameplayTag ModuleTag) const
{
	return ModuleTag.IsValid() && ActiveModules.HasTagExact(ModuleTag);
}

void UShipStateComponent::AdvanceDay(int32 Days)
{
	if (Days <= 0) return;

	Hunger = FMath::Max(0.f, Hunger - HungerPerDay * Days);
	Thirst = FMath::Max(0.f, Thirst - ThirstPerDay * Days);

	OnVitalsChanged.Broadcast();
}


bool UShipStateComponent::ConsumeFood(float RestoreAmount) {
	if (FoodUnits > 0) { 
		FoodUnits--; 
		Hunger = FMath::Min(100.f, Hunger + RestoreAmount);
		OnSuppliesChanged.Broadcast(); 
		OnVitalsChanged.Broadcast(); 
		return true; 
	} 
	return false; } 

bool UShipStateComponent::ConsumeWater(float RestoreAmount) {
	if (WaterUnits > 0) { 
		WaterUnits--; 
		Thirst = FMath::Min(100.f, Thirst + RestoreAmount);
		OnSuppliesChanged.Broadcast(); 
		OnVitalsChanged.Broadcast(); 
		return true; 
	} 
	return false;
}
