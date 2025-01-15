// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Graph/FlowGraphPinFactory.h"
#include "Graph/FlowGraphSchema.h"
#include "Graph/FlowGraphSettings.h"
#include "Graph/Nodes/FlowGraphNode.h"
#include "Graph/Widgets/SFlowGraphNode.h"
#include "Nodes/FlowNode.h"
#include "Nodes/FlowPin.h"

#include "NodeFactory.h"
#include "SGraphPin.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowGraphPinFactory)

//////////////////////////////////////////////////////////////////////////
// FFlowGraphPinFactory

TSharedPtr<SGraphPin> FFlowGraphPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (!InPin->GetSchema()->IsA<UFlowGraphSchema>())
	{
		// Limit pin widget creation to FlowGraph schemas 
		return nullptr;
	}

	const UFlowGraphNode* FlowGraphNode = Cast<UFlowGraphNode>(InPin->GetOwningNode());

	// Create the widget for a Flow 'Exec'-style pin
	if (FlowGraphNode && FFlowPin::IsExecPinCategory(InPin->PinType.PinCategory))
	{
		const TSharedPtr<SGraphPin> NewPinWidget = SNew(SFlowGraphPinExec, InPin);

		const UFlowNode* FlowNode = Cast<UFlowNode>(FlowGraphNode->GetFlowNodeBase());

		if (!UFlowGraphSettings::Get()->bShowDefaultPinNames && IsValid(FlowNode))
		{
			if (InPin->Direction == EGPD_Input)
			{
				// Pin array can have pins with name None, which will not be created. We need to check if array have only one valid pin
				if (GatherValidPinsCount(FlowNode->GetInputPins()) == 1 && InPin->PinName == UFlowNode::DefaultInputPin.PinName)
				{
					NewPinWidget->SetShowLabel(false);
				}
			}
			else
			{
				// Pin array can have pins with name None, which will not be created. We need to check if array have only one valid pin
				if (GatherValidPinsCount(FlowNode->GetOutputPins()) == 1 && InPin->PinName == UFlowNode::DefaultOutputPin.PinName)
				{
					NewPinWidget->SetShowLabel(false);
				}
			}
		}

		return NewPinWidget;
	}

	// For data pins, give the K2 (blueprint) node factory an opportunity to create the widget
	TSharedPtr<SGraphPin> K2PinWidget = FNodeFactory::CreateK2PinWidget(InPin);
	if (K2PinWidget.IsValid())
	{
		return K2PinWidget;
	}

	return nullptr;
}

int32 FFlowGraphPinFactory::GatherValidPinsCount(const TArray<FFlowPin>& Pins)
{
	int32 Count = 0;
	for (const FFlowPin& Pin : Pins)
	{
		if (Pin.IsValid())
		{
			++Count;
		}
	}

	return Count;
}

//////////////////////////////////////////////////////////////////////////
// UFlowK2SchemaSubclassForAccess

void UFlowK2SchemaSubclassForAccess::AssertPinCategoryNames()
{
	// Assert that the FFlowPin aliases for the UEdGraphSchema_K2 PC_* FNames match exactly
	// (since we will leverage some K2 functions that key off these names)
	checkf(FFlowPin::PC_Exec == UEdGraphSchema_K2::PC_Exec, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Boolean == UEdGraphSchema_K2::PC_Boolean, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Byte == UEdGraphSchema_K2::PC_Byte, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Class == UEdGraphSchema_K2::PC_Class, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Int == UEdGraphSchema_K2::PC_Int, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Int64 == UEdGraphSchema_K2::PC_Int64, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Float == UEdGraphSchema_K2::PC_Float, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Double == UEdGraphSchema_K2::PC_Double, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Name == UEdGraphSchema_K2::PC_Name, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Object == UEdGraphSchema_K2::PC_Object, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_String == UEdGraphSchema_K2::PC_String, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Text == UEdGraphSchema_K2::PC_Text, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Struct == UEdGraphSchema_K2::PC_Struct, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
	checkf(FFlowPin::PC_Enum == UEdGraphSchema_K2::PC_Enum, TEXT("Flow PC_* aliases should match Epic's K2 PC_* names exactly"));
}
