// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ImGuiBlueprintLibrary.h"
#include "K2Node.h"
#include "K2Node_ImGuiBeginBase.generated.h"

/**
 *  Base class for functions like Begin(), BeginMainMenuBar(), etc.
 */
UCLASS()
class IMGUIBLUEPRINTUNCOOKEDONLY_API UK2Node_ImGuiBeginBase : public UK2Node
{
	GENERATED_BODY()

	virtual FName GetBeginFunction() const
	{
		return GET_FUNCTION_NAME_CHECKED(UImGuiBlueprintLibrary, ImGui_Begin);
	}

	virtual FName GetLabelPinName() const
	{
		return FName("Name"); // apparently only a thing on Begin()
	}

	virtual bool IsLabelRequired() const
	{
		return true;
	}

	virtual FName GetEndFunction() const
	{
		return GET_FUNCTION_NAME_CHECKED(UImGuiBlueprintLibrary, ImGui_End);
	}

	// True if an End call is always required.
	// False if only need to call End if it's activated
	virtual bool AlwaysRequiresEndCall() const
	{
		return true; // Only true for Begin() and nothing else
	}

	// K2Node API
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	UE_NODISCARD FText GetMenuCategory() const override;

	// EdGraphNode API
	void AllocateDefaultPins() override;
	void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	FText GetTooltipText() const override;

private:

	static const FName OnActiveExecutionPinName;

	static const FName NamePinName;
};
