// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPSTest : ModuleRules
{
	public FPSTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags", "UMG", "Cascade", "Niagara" });
	}
}
