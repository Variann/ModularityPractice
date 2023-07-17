using UnrealBuildTool;

public class TagFactsEditor : ModuleRules
{
    public TagFactsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "DataRegistryEditor",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "UnrealEd",
                "PropertyEditor", 
                "TagFacts", 
                "GameplayTagsEditor",
                "GameplayTags", "DataRegistry"
            }
        );
    }
}