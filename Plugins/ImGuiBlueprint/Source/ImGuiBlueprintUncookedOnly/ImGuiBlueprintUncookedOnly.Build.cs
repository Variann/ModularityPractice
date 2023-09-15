using UnrealBuildTool;

public class ImGuiBlueprintUncookedOnly : ModuleRules
{
    public ImGuiBlueprintUncookedOnly(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "ImGuiBlueprint"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "KismetCompiler",
                "GraphEditor",
                "Kismet",
                "UnrealEd",
                "BlueprintGraph",
                "BlueprintEditorLibrary",
                "ImGuiBlueprint"
            }
        );
    }
}