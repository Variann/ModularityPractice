// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#include "EditorAssistant.h"

#include "EditorAssistantThumbnailRenderer.h"
#include "Config/DS_EditorAssistantSettings.h"
#include "Interfaces/IPluginManager.h"


#define LOCTEXT_NAMESPACE "FEditorAssistantModule"

void FEditorAssistantModule::StartupModule()
{
	GetDefault<UDS_EditorAssistantSettings>()->AddContentBrowserContextMenuExtensions();

	//Disable the old renderers
	UThumbnailManager::Get().UnregisterCustomRenderer(UBlueprint::StaticClass());
	UThumbnailManager::Get().UnregisterCustomRenderer(UDataAsset::StaticClass());

	//Enable our custom renderer
	UThumbnailManager::Get().RegisterCustomRenderer(UBlueprint::StaticClass(), UEditorAssistantThumbnailRenderer::StaticClass());
	UThumbnailManager::Get().RegisterCustomRenderer(UDataAsset::StaticClass(), UEditorAssistantThumbnailRenderer::StaticClass());

	AutoPopulateContextMenu();
}

void FEditorAssistantModule::ShutdownModule()
{
}

void FEditorAssistantModule::AutoPopulateContextMenu()
{
	UDS_EditorAssistantSettings* EA_Settings = Cast<UDS_EditorAssistantSettings>(UDS_EditorAssistantSettings::StaticClass()->GetDefaultObject());
	
	/**This is not the prettiest code, it might not be the fastest code, it might not
	 * be the most readable code. But... It works, it's not annoyingly slow, and since
	 * this is a editor tool, I'm not going to spend too much time on this.
	 *
	 * TL;DR: Attempt to find out what plugin, if any, a class is from.
	 * Find the category it wants to go into.
	 * Make sure all classes from a specific plugin go into the same sub entry.
	 * If no category is found, create the category.*/
	for(TObjectIterator<UClass> CurrentClass; CurrentClass; ++CurrentClass)
	{
		if(!CurrentClass->HasMetaData("ContextMenuCategory"))
		{
			continue;
		}
		
		FString PluginName;
		const FString& ModuleDependency = FPackageName::GetShortName(CurrentClass->GetPackage()->GetName());

		TArray<FModuleStatus> ModuleStatuses;
		const FModuleManager& ModuleManager = FModuleManager::Get();
		ModuleManager.QueryModules(ModuleStatuses);
		for (FModuleStatus& ModuleStatus : ModuleStatuses)
		{
			if (ModuleStatus.bIsLoaded && ModuleStatus.Name == ModuleDependency)
			{
				// this is the module's plugin
				const TSharedPtr<IPlugin>& OwnerPlugin = IPluginManager::Get().GetModuleOwnerPlugin(*ModuleDependency);
				if(OwnerPlugin.IsValid())
				{
					PluginName = OwnerPlugin->GetName();
				}
			}
		}
		
		//Create the entry
		FContextMenuEntry Entry;

		//Start finding the metadata and fill in the entry data.
		
		//If class isn't from a plugin, just use raw metadata
		if(PluginName.IsEmpty())
		{
			Entry.Path = "Game|" + CurrentClass->GetMetaDataText("ContextMenuEntryName").ToString();
		}
		else
		{
			Entry.Path = PluginName + "|" + CurrentClass->GetMetaDataText("ContextMenuEntryName").ToString();
		}
		
		Entry.Prefix = CurrentClass->GetMetaDataText("ContextMenuPrefix").ToString();
		Entry.AssetName = CurrentClass->GetMetaDataText("ContextMenuAssetName").ToString();
		Entry.Postfix = CurrentClass->GetMetaDataText("ContextMenuPostfix").ToString();
		Entry.ParentClass = *CurrentClass;
		
		FText Category = CurrentClass->GetMetaDataText("ContextMenuCategory");
		bool CreateCategory = !EA_Settings->Categories.Contains(Category);

		//Find the category for the entry.
		for(auto& CurrentCategory : EA_Settings->Categories)
		{
			if(!CurrentCategory.CategoryName.EqualTo(Category))
			{
				continue;
			}

			//Category already has this class, skip it.
			//Might happen due to manual addition
			if(CurrentCategory.Entries.Contains(*CurrentClass))
			{
				break;
			}
			
			if(CurrentCategory.CategoryName.EqualTo(Category))
			{
				CurrentCategory.Entries.Add(Entry);
				CreateCategory = false;
			}
			//If category wasn't found, and it's invalid, metadata hasn't been set up properly.
			else if(!Category.IsEmpty())
			{
				//Category wasn't found, append it to the categories after this loop.
				FContextMenuCategory ContextMenuCategory;
				ContextMenuCategory.CategoryName = CurrentClass->GetMetaDataText("ContextMenuCategory");
				ContextMenuCategory.Entries.Add(Entry);
				
				EA_Settings->Categories.Add(ContextMenuCategory);
				CreateCategory = false;
			}
			else
			{
				CreateCategory = true;
			}
		}

		if(CreateCategory)
		{
			FContextMenuCategory ContextMenuCategory;
			ContextMenuCategory.CategoryName = CurrentClass->GetMetaDataText("ContextMenuCategory");
			ContextMenuCategory.Entries.Add(Entry);
				
			EA_Settings->Categories.Add(ContextMenuCategory);
			CreateCategory = true;
		}
	}
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEditorAssistantModule, EditorAssistant)