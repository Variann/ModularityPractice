// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ModularityPractice : ModuleRules
{
	public ModularityPractice(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "Flow" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
