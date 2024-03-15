// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetToolsModule.h"
#include "ClassIconFinder.h"
#include "ContentBrowserModule.h"
#include "EditorAssistantThumbnailRenderer.h"
#include "EditorUtilityWidget.h"
#include "IContentBrowserSingleton.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/DeveloperSettings.h"
#include "Factories/BlueprintFactory.h"
#include "Factories/DataAssetFactory.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "DS_EditorAssistantSettings.generated.h"

struct FMenuItem : TSharedFromThis<FMenuItem>
{
	/**Credit: A lot of this code was originally taken from GASCompanion, made by MKLabs.
	 * Although heavily modified, I would not have been able to reach this point without
	 * researching his plugin first.
	 * - Varian*/
	
	TMap<FString, TSharedPtr<FMenuItem>> SubItems;

	FString Prefix;
	FString Postfix;
	FString DefaultAssetName;
	FText Tooltip;
	TSubclassOf<UObject> ParentClass;

	static void BuildMenus_r(TSharedPtr<FMenuItem> Item, FMenuBuilder& MenuBuilder, TArray<FString> SelectedPaths)
	{
		for (auto CurrentItem : Item->SubItems)
		{
			TSharedPtr<FMenuItem> SubItem  = CurrentItem.Value;
			FString ItemName = CurrentItem.Key;

			// Add a submenu if this element has sub items
			if (SubItem->SubItems.Num() > 0)
			{
				MenuBuilder.AddSubMenu(
					FText::FromString(ItemName),
					FText::FromString(ItemName),
					FNewMenuDelegate::CreateLambda([SubItem, SelectedPaths](FMenuBuilder& SubMenuBuilder)
					{
						BuildMenus_r(SubItem, SubMenuBuilder, SelectedPaths);
					})
				);
			}

			// Add the menu entry to create the class asset.
			if (SubItem->ParentClass.Get() != nullptr)
			{
				//In case the asset does not have a thumbnail, use the default thumbnail for the class.
				const FSlateBrush* ConstBrush = FClassIconFinder::FindThumbnailForClass(SubItem->ParentClass);
				FSlateBrush* Brush = nullptr;

				//Conveniently enough, this will get the custom thumbnail override.
				if(UTexture2D* FoundThumbnail = UEditorAssistantThumbnailRenderer::FindThumbnailOverrideForObject(SubItem->ParentClass.GetDefaultObject()))
				{
					Brush = new FSlateBrush();
					*Brush = UWidgetBlueprintLibrary::MakeBrushFromTexture(FoundThumbnail, 32, 32);
				}

				/**MenuBuilder.AddMenuEntry only accepts FSlateIcon, which does not seem to have any way of
				 * accepting a UTexture2D*. For now, I am manually creating something that looks and feels
				 * similar. This makes a overlay with a button, image and text and lays them out
				 * to look and behave just like the normal buttons..*/
				MenuBuilder.BeginSection("EA_ContentBrowserContextMenu");
				{
					//Main content browser buttons have some extra padding on the left
					int32 LeftPadding = 10;
					MenuBuilder.AddWidget(
						SNew(SOverlay)
						+ SOverlay::Slot()
						.HAlign(HAlign_Fill)
						[
							SNew(SButton)
							.ButtonStyle(FAppStyle::Get(), "Menu.Button")
							.ToolTipText(SubItem->ParentClass->GetToolTipText())
							.OnClicked_Lambda([=]()
							{
								if(SelectedPaths.Num() > 0)
								{
									//Get the modules
									const FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
									const FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
									
									//Get the correct content browser.
									TArray<FAssetData> SelectedAssets;
									ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
									
									//Set up the new items path, name and find out if another asset has the same name. If so, then change it to a unique asset name.
									FString PackageName = SelectedPaths[0];
									FString AssetName = SubItem->Prefix + SubItem->DefaultAssetName + SubItem->Postfix;
									const FString DefaultFullPath = PackageName / AssetName;
									AssetToolsModule.Get().CreateUniqueAssetName(DefaultFullPath, TEXT(""), PackageName, AssetName);
									
									//Handle blueprint creation.
									if(FKismetEditorUtilities::CanCreateBlueprintOfClass(SubItem->ParentClass) && !SubItem->ParentClass->IsChildOf(UDataAsset::StaticClass()))
									{
										UBlueprintFactory* BlueprintFactory = NewObject<UBlueprintFactory>();
										BlueprintFactory->ParentClass = SubItem->ParentClass;
										
										AssetToolsModule.Get().CreateUniqueAssetName(DefaultFullPath, TEXT(""), PackageName, AssetName);
										UBlueprint* Asset =	Cast<UBlueprint>(AssetToolsModule.Get().CreateAsset(*AssetName, SelectedPaths[0], UBlueprint::StaticClass(), BlueprintFactory));
										if(Asset)
										{
											Asset->Modify();
										}
									}
									//Handle data asset creation.
									else if(SubItem->ParentClass->IsChildOf(UDataAsset::StaticClass()))
									{
										UDataAssetFactory* DataAssetFactory = NewObject<UDataAssetFactory>();
										DataAssetFactory->DataAssetClass = SubItem->ParentClass;
										
										AssetToolsModule.Get().CreateUniqueAssetName(DefaultFullPath, TEXT(""), PackageName, AssetName);
										UDataAsset* Asset = Cast<UDataAsset>(AssetToolsModule.Get().CreateAsset(*AssetName, SelectedPaths[0], UDataAsset::StaticClass(), DataAssetFactory));
										if(Asset)
										{
											Asset->Modify();
										}
									}
								}
								//MenuBuilder doesn't have a built in function to call to close the menu.
                                //Overkill solution, but dismiss all menus.
                                ////There is DismissMenu, but I'm not sure how to use it with FMenuBuilder
								FSlateApplication::Get().DismissAllMenus();
								return FReply::Handled();
							}
							)
						]
						+ SOverlay::Slot()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Fill)
						.Padding(LeftPadding, 0, 0, 0)
						[
							SNew(SImage)
							.Image(Brush ? Brush : ConstBrush)
							.Visibility(EVisibility::HitTestInvisible)
							.DesiredSizeOverride(FVector2D(42, 42))
						]
						+ SOverlay::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						.Padding(LeftPadding + 48, 0, 10, 0)
						[
							SNew(STextBlock)
							.TextStyle(FAppStyle::Get(), "Menu.Label")
							.Text(FText::FromString(ItemName))
							.ColorAndOpacity(FLinearColor::White)
							.Visibility(EVisibility::HitTestInvisible)
						],
						FText()
						);
					MenuBuilder.EndSection();
				};
			}
		}
	}
};

USTRUCT()
struct FContextMenuEntry
{
	GENERATED_BODY()

	/**Path to this entry. Use "|" for sub categories. For example, ClassParent|Child1*/
	UPROPERTY(Category = "Settings", EditAnywhere)
	FString Path = FString();

	/**Prefix added to the asset when created.*/
	UPROPERTY(Category = "Settings", EditAnywhere)
	FString Prefix = FString();

	/**Default name for the asset when created, what name should automatically be added, if any.
	 * I advise leaving this empty and always filling in the @Prefix so when you
	 * press F2 to rename it, you don't have to erase anything and can just instantly
	 * start filling in the name. This also promotes people to leave real names
	 * on their assets and not default names.*/
	UPROPERTY(Category = "Settings", EditAnywhere)
	FString AssetName = FString();

	/**Postfix added to the asset when created.*/
	UPROPERTY(Category = "Settings", EditAnywhere)
	FString Postfix = FString();

	/**Parent class to assign to the asset.
	 * This can ONLY be either a data asset or an asset
	 * that is Blueprintable.*/
	UPROPERTY(Category = "Settings", EditAnywhere)
	TSubclassOf<UObject> ParentClass = nullptr;

	FORCEINLINE bool operator ==(const TSubclassOf<UObject> Class) const
	{
		return ParentClass == Class;
	}
};

USTRUCT()
struct FContextMenuCategory
{
	GENERATED_BODY()

	/**Name of the category to show in the context menu.*/
	UPROPERTY(Category = "Settings", EditAnywhere)
	FText CategoryName;

	/**Tooltip to show when hovering over this category.*/
	UPROPERTY(Category = "Settings", EditAnywhere)
	FText Tooltip;

	UPROPERTY(Category = "Settings", EditAnywhere, meta = (TitleProperty = "Path"))
	TArray<FContextMenuEntry> Entries;

	FORCEINLINE bool operator ==(const FText& InName) const
	{
		return CategoryName.EqualTo(InName);
	}
};

USTRUCT(Blueprintable)
struct FClassQuickReferences
{
	GENERATED_BODY()

	UPROPERTY(Category = "Utility Widgets", EditAnywhere)
	TArray<TSoftClassPtr<UObject>> Classes;
};

USTRUCT(Blueprintable)
struct FAssetQuickReferences
{
	GENERATED_BODY()

	UPROPERTY(Category = "Utility Widgets", EditAnywhere)
	TArray<TSoftObjectPtr<UObject>> Assets;
};

/**
 * 
 */
UCLASS(Config = EditorPerProjectUserSettings, meta = (DisplayName = "Editor Assistant"))
class EDITORASSISTANT_API UDS_EditorAssistantSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UDS_EditorAssistantSettings()
	{
		CategoryName = TEXT("Plugins");
		SectionName = TEXT("Editor Assistant");
	}

	/**Categories to add to the content browser.*/
	UPROPERTY(Category = "Content Browser", config, EditAnywhere, meta = (TitleProperty = "CategoryName"))
	TArray<FContextMenuCategory> Categories;

	/**What classes should have their thumbnails overriden with a custom
	 * texture? This also applies to children of any classes in this map.*/
	UPROPERTY(Category = "Content Browser", config, EditAnywhere)
	TMap<TSoftClassPtr<UObject>, TSoftObjectPtr<UTexture2D>> ThumbnailOverrides;

	/**What widgets to add as an option in the main window editor's dropdown.*/
	UPROPERTY(Category = "Utility Widgets", config, EditAnywhere)
	TArray<TSoftClassPtr<UEditorUtilityWidget>> EditorUtilityWidgets;

	/**Quick way of retrieving specific classes and sorting them into categories.*/
	UPROPERTY(Category = "Utility Widgets", config, EditAnywhere)
	TMap<FName, FClassQuickReferences> ClassQuickReferenceCategories;

	/**Quick way of retrieving specific classes and sorting them into categories.*/
	UPROPERTY(Category = "Utility Widgets", config, EditAnywhere)
	TMap<FName, FAssetQuickReferences> AssetQuickReferenceCategories;

	UFUNCTION(Category = "Editor Assistant", BlueprintCallable)
	static TArray<TSubclassOf<UObject>> GetClassesFromCategory(FName Category);

	UFUNCTION(Category = "Editor Assistant", BlueprintCallable)
	static TArray<UObject*> GetAssetsFromCategory(FName Category);
	
	void AddContentBrowserContextMenuExtensions() const;

	static void TopMenuBuild(FMenuBuilder& TopMenuBuilder, const FText InMenuLabel, const FText InMenuTooltip, const TArray<FString> SelectedPaths, TArray<FContextMenuEntry> Entries);

	//TODO V: Add #if WITH_EDITOR. It was removed because for some reason it was not working for the CPP file when packaging
	UFUNCTION(Category = "EditorAssistant|Editor", BlueprintCallable)
	static FSlateBrush GetSlateBrush(FName Style);
};
