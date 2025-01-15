// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"

#include "FlowGraphPinFactory.generated.h"

struct FFlowPin;

class FFlowGraphPinFactory : public FGraphPanelPinFactory
{
public:
	// FGraphPanelPinFactory interface
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override;
	// --

	static int32 GatherValidPinsCount(const TArray<FFlowPin>& Pins);
};

// Thin subclass of UEdGraphSchema_K2 to gain access to PC_* defines so we can assert their values
UCLASS()
class UFlowK2SchemaSubclassForAccess : public UEdGraphSchema_K2
{
	GENERATED_BODY()

public:
	static void AssertPinCategoryNames();
};
