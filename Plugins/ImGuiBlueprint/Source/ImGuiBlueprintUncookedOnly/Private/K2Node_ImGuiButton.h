// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_ImGuiButton.generated.h"

/**
 * 
 */
UCLASS()
class IMGUIBLUEPRINTUNCOOKEDONLY_API UK2Node_ImGuiButton : public UK2Node
{
	GENERATED_BODY()

	// K2Node API
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	UE_NODISCARD FText GetMenuCategory() const override;

	// EdGraphNode API
	void AllocateDefaultPins() override;
	void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	FText GetTooltipText() const override;

private:

	static const FName OnClickExecutionPinName;

	static const FName NamePinName;
	static const FName SizePinName;
};
