// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Config/DS_EditorAssistantSettings.h"
#include "ContentBrowserModule.h"


TArray<TSubclassOf<UObject>> UDS_EditorAssistantSettings::GetClassesFromCategory(FName Category)
{
	UDS_EditorAssistantSettings* EA_Settings = Cast<UDS_EditorAssistantSettings>(UDS_EditorAssistantSettings::StaticClass()->GetDefaultObject());

	TArray<TSubclassOf<UObject>> ReturnClasses;
	if(!EA_Settings->ClassQuickReferenceCategories.Contains(Category))
	{
		return ReturnClasses;
	}

	TArray<TSoftClassPtr<UObject>> FoundClasses = EA_Settings->ClassQuickReferenceCategories.Find(Category)->Classes;
	
	for(auto& CurrentClass : FoundClasses)
	{
		if(CurrentClass.ToSoftObjectPath().IsValid())
		{
			ReturnClasses.Add(CurrentClass.LoadSynchronous());
		}
	}

	return ReturnClasses;
}

TArray<UObject*> UDS_EditorAssistantSettings::GetAssetsFromCategory(FName Category)
{
	UDS_EditorAssistantSettings* EA_Settings = Cast<UDS_EditorAssistantSettings>(UDS_EditorAssistantSettings::StaticClass()->GetDefaultObject());

	TArray<TSoftObjectPtr<UObject>> FoundAssets = EA_Settings->AssetQuickReferenceCategories.Find(Category)->Assets;
	TArray<UObject*> ReturnAssets;
	
	for(auto& CurrentClass : FoundAssets)
	{
		if(CurrentClass.ToSoftObjectPath().IsValid())
		{
			ReturnAssets.Add(CurrentClass.LoadSynchronous());
		}
	}

	return ReturnAssets;
}

void UDS_EditorAssistantSettings::AddContentBrowserContextMenuExtensions() const
{
	TSharedPtr<FUICommandList> CommandBindings = MakeShareable(new FUICommandList());
	//Fetch the content browser module.
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	//I believe the lambda requires a weak object pointer, not sure. This is how it's done in most references I can find.
	TWeakObjectPtr<const UDS_EditorAssistantSettings> WeakSettingsRef(this);

	ContentBrowserModule.GetAllAssetContextMenuExtenders().Add(FContentBrowserMenuExtender_SelectedPaths::CreateLambda([WeakSettingsRef](const TArray<FString>& SelectedPaths)
	{
		TSharedRef<FExtender> Extender = MakeShared<FExtender>();

		for(auto& CurrentMenuEntry : WeakSettingsRef->Categories)
		{
			if(WeakSettingsRef.IsValid())
			{
				const FText MenuLabel = CurrentMenuEntry.CategoryName;
				const FText MenuTooltip = CurrentMenuEntry.Tooltip;
				Extender->AddMenuExtension(
				"ContentBrowserNewAdvancedAsset",
				EExtensionHook::After,
				TSharedPtr<FUICommandList>(),
				FMenuExtensionDelegate::CreateStatic(&TopMenuBuild, MenuLabel, MenuTooltip, SelectedPaths, CurrentMenuEntry.Entries));
			}
		}

		return Extender;
	}));
}

void UDS_EditorAssistantSettings::TopMenuBuild(FMenuBuilder& TopMenuBuilder, const FText InMenuLabel,
                                               const FText InMenuTooltip, const TArray<FString> SelectedPaths, TArray<FContextMenuEntry> Entries)
{
	if (Entries.Num() == 0)
	{
		return;
	}

	TopMenuBuilder.AddSubMenu(
		InMenuLabel,
		InMenuTooltip,
		FNewMenuDelegate::CreateLambda([SelectedPaths, Entries](FMenuBuilder& GETopMenuBuilder)
		{
			// Loop through our Definitions and build the menu items.
			const TSharedPtr<FMenuItem> RootItem = MakeShareable(new FMenuItem);
			for (const auto& CurrentEntry : Entries)
			{
					
				if (CurrentEntry.ParentClass.Get() == nullptr)
				{
					continue;
				}

				TArray<FString> CategoryStrings;
				CurrentEntry.Path.ParseIntoArray(CategoryStrings, TEXT("|"), true);

				FMenuItem* CurrentItem = RootItem.Get();
				for (int32 idx=0; idx < CategoryStrings.Num(); ++idx)
				{
					FString& Str = CategoryStrings[idx];
					TSharedPtr<FMenuItem>& DestItem = CurrentItem->SubItems.FindOrAdd(Str);
					if (!DestItem.IsValid())
					{
						DestItem = MakeShareable(new FMenuItem);
					}
					CurrentItem = DestItem.Get();
				}
		
				CurrentItem->Prefix = CurrentEntry.Prefix;
				CurrentItem->Postfix = CurrentEntry.Postfix;
				CurrentItem->DefaultAssetName = CurrentEntry.AssetName;
				CurrentItem->Tooltip = CurrentEntry.ParentClass->GetToolTipText();
				CurrentItem->ParentClass = CurrentEntry.ParentClass.Get();
			}
			// Build menu delegates based on our data
			FMenuItem::BuildMenus_r(RootItem, GETopMenuBuilder, SelectedPaths);
		})
	);
}

FSlateBrush UDS_EditorAssistantSettings::GetSlateBrush(FName Style)
{
	FSlateBrush Brush = *FSlateIcon(FAppStyle::GetAppStyleSetName(), Style).GetIcon();
	return Brush;
}
