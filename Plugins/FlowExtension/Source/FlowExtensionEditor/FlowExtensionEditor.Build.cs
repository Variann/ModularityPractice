using UnrealBuildTool;

public class FlowExtensionEditor : ModuleRules
{
    public FlowExtensionEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "EditorStyle", "KismetWidgets", "FlowExtension",
                "EditorSubsystem",
                "Flow",
                "FlowEditor",
                "MessageLog"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "GraphEditor"
            }
        );
    }
}