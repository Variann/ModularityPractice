// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_ImGuiBeginBase.h"
#include "K2Node_ImGuiMainMenuBar.generated.h"

/**
 * 
 */
UCLASS()
class IMGUIBLUEPRINTUNCOOKEDONLY_API UK2Node_ImGuiMainMenuBar : public UK2Node_ImGuiBeginBase
{
	GENERATED_BODY()

	// UK2Node_ImGuiBeginBase interface
	FName GetBeginFunction() const override
	{
		return GET_FUNCTION_NAME_CHECKED(UImGuiBlueprintLibrary, ImGui_BeginMainMenuBar);
	}

	FName GetEndFunction() const override
	{
		return GET_FUNCTION_NAME_CHECKED(UImGuiBlueprintLibrary, ImGui_EndMainMenuBar);
	}

	bool IsLabelRequired() const override
	{
		return false;
	}

	bool AlwaysRequiresEndCall() const override
	{
		return false;
	}

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return NSLOCTEXT("K2Node_ImGuiMainMenuBar", "NodeTitle", "ImGui Main Menu Bar");
	}

	virtual FText GetTooltipText() const override
	{
		return NSLOCTEXT("K2Node_ImGuiMainMenuBar", "NodeTooltip", "ImGui Main Menu Bar");
	}

	virtual FText GetMenuCategory() const override
	{
		return NSLOCTEXT("K2Node_ImGuiMainMenuBar", "MenuCategory", "ImGui|Menus");
	}
};
