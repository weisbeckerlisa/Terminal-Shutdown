// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TerminalShutdown : ModuleRules
{
	public TerminalShutdown(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
            "GameplayTags",
            "DeveloperSettings"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"TerminalShutdown",
			"TerminalShutdown/Variant_Horror",
			"TerminalShutdown/Variant_Horror/UI",
			"TerminalShutdown/Variant_Shooter",
			"TerminalShutdown/Variant_Shooter/AI",
			"TerminalShutdown/Variant_Shooter/UI",
			"TerminalShutdown/Variant_Shooter/Weapons",
			"TerminalShutdown/EventSystem"
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
