﻿// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
//using System.IO;

//这个模块只是用来创建不同时间段加载用的, 所以只用依赖这一个
public class doodleEditor : ModuleRules
{
	public doodleEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
                // ... add public include paths required here ...
            }
			);


		PrivateIncludePaths.AddRange(
			new string[] {
                // ... add other private include paths required here ...
            }
			);



		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"ContentBrowser",
				"EditorScriptingUtilities",

			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"doodle"
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
                // ... add any modules that your module loads dynamically here ...
            }
			);
	}
}
