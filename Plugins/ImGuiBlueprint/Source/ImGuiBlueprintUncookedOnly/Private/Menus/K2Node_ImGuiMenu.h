// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_ImGuiBeginBase.h"
#include "K2Node_ImGuiMenu.generated.h"

/**
 * 
 */
UCLASS()
class IMGUIBLUEPRINTUNCOOKEDONLY_API UK2Node_ImGuiMenu : public UK2Node_ImGuiBeginBase
{
	GENERATED_BODY()
	// UK2Node_ImGuiBeginBase interface
	FName GetBeginFunction() const override
	{
		return GET_FUNCTION_NAME_CHECKED(UImGuiBlueprintLibrary, ImGui_BeginMenu);
	}

	FName GetEndFunction() const override
	{
		return GET_FUNCTION_NAME_CHECKED(UImGuiBlueprintLibrary, ImGui_EndMenu);
	}

	FName GetLabelPinName() const override
	{
		return FName("Label");
	}

	bool IsLabelRequired() const override
	{
		return true;
	}

	bool AlwaysRequiresEndCall() const override
	{
		return false;
	}

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return NSLOCTEXT("K2Node_ImGuiMenu", "NodeTitle", "ImGui Menu");
	}

	virtual FText GetTooltipText() const override
	{
		return NSLOCTEXT("K2Node_ImGuiMenu", "NodeTooltip", "ImGui Menu");
	}

	virtual FText GetMenuCategory() const override
	{
		return NSLOCTEXT("K2Node_ImGuiMenu", "MenuCategory", "ImGui|Menus");
	}
};
