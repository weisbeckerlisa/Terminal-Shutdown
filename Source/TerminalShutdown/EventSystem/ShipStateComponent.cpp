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
