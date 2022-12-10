// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InventoryAndCrafting : ModuleRules
{
    public InventoryAndCrafting(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });

        PublicIncludePaths.AddRange(new string[]
        {
            "InventoryAndCrafting/Public/Pickup",
            "InventoryAndCrafting/Public/Player"
        });
    }
}
