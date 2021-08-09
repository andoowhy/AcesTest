using UnrealBuildTool;

public class AcesEditor : ModuleRules
{
	public AcesEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"BlueprintGraph",
			"Slate",
			"SlateCore",
			"EditorStyle",
			"GraphEditor",
			"ToolMenus",
			"KismetCompiler",
			"UnrealEd",
			"Aces"
		});
	}
}